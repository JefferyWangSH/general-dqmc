#include "dynamic_measure.h"
#include "hubbard.h"

Measure::DynamicMeasure::DynamicMeasure(const int &nbin) {
    this->nbin = nbin;
}

void Measure::DynamicMeasure::resize(const int &_nbin) {
    this->nbin = _nbin;
}

void Measure::DynamicMeasure::initial(const Model::Hubbard &hubbard) {
    this->sign.set_size_of_bin(this->nbin);
    this->matsubara_greens.set_size_of_bin(this->nbin);
    this->density_of_states.set_size_of_bin(this->nbin);
    this->superfluid_stiffness.set_size_of_bin(this->nbin);
    // this->current_current_corr.set_size_of_bin(this->nbin);

    this->sign.set_zero_element(0.0);
    this->matsubara_greens.set_zero_element(Eigen::VectorXd(hubbard.lt));
    this->density_of_states.set_zero_element(Eigen::VectorXd(hubbard.lt));
    this->superfluid_stiffness.set_zero_element(0.0);
    // this->current_current_corr.set_zero_elements(Eigen::MatrixXd(hubbard.ls, hubbard.ls));

    this->sign.allocate();
    this->matsubara_greens.allocate();
    this->density_of_states.allocate();
    this->superfluid_stiffness.allocate();
    // this->current_current_corr.allocate();
}

void Measure::DynamicMeasure::clear_temporary(const Model::Hubbard &hubbard) {
    this->sign.clear_temporary();
    this->matsubara_greens.clear_temporary();
    this->density_of_states.clear_temporary();
    this->superfluid_stiffness.clear_temporary();
    // this->current_current_corr.clear_temporary();
}

void Measure::DynamicMeasure::time_displaced_measure(const Model::Hubbard &hubbard) {
    this->sign.tmp_value() += hubbard.config_sign;
    for (int l = 0; l < hubbard.lt; ++l) {
        this->measure_matsubara_greens(l, hubbard);
        this->measure_density_of_states(l, hubbard);
    }
    this->measure_superfluid_stiffness(hubbard);

    ++this->sign;
    ++this->matsubara_greens;
    ++this->density_of_states;
    ++this->superfluid_stiffness;
}

void Measure::DynamicMeasure::normalize_stats(const Model::Hubbard &hubbard) {
    this->sign.tmp_value() /= this->sign.counts();
    this->matsubara_greens.tmp_value() /= this->matsubara_greens.counts() * this->sign.tmp_value();
    this->density_of_states.tmp_value() /= this->density_of_states.counts() * this->sign.tmp_value();
    this->superfluid_stiffness.tmp_value() /= this->superfluid_stiffness.counts() * this->sign.tmp_value();
    // this->current_current_corr.tmp_value() /= this->current_current_corr.counts() * this->sign.tmp_value();
}

void Measure::DynamicMeasure::write_stats_to_bins(const int &bin, const Model::Hubbard &hubbard) {
    this->sign.bin_data()[bin] = this->sign.tmp_value();
    this->matsubara_greens.bin_data()[bin] = this->matsubara_greens.tmp_value();
    this->density_of_states.bin_data()[bin] = this->density_of_states.tmp_value();
    this->superfluid_stiffness.bin_data()[bin] = this->superfluid_stiffness.tmp_value();
    // this->current_current_corr.bin_data()[bin] = this->current_current_corr.tmp_value();
}

void Measure::DynamicMeasure::measure_matsubara_greens(const int &t, const Model::Hubbard &hubbard) {
    assert( t >= 0 && t < hubbard.lt );
    // factor 1/2 comes from two degenerate spin states
    const Eigen::MatrixXd gt0 = ( t == 0 )?
              0.5 * (hubbard.vec_green_tt_up[hubbard.lt-1] + hubbard.vec_green_tt_up[hubbard.lt-1])
            : 0.5 * (hubbard.vec_green_t0_up[t-1] + hubbard.vec_green_t0_up[t-1]);

    // base point i
    for (int xi = 0; xi < hubbard.ll; ++xi) {
        for (int yi = 0; yi < hubbard.ll; ++yi) {
            const int i = xi + hubbard.ll * yi;
            // displacement
            for (int dx = 0; dx < hubbard.ll; ++dx) {
                for (int dy = 0; dy < hubbard.ll; ++dy) {
                    const int j = (xi + dx) % hubbard.ll + hubbard.ll * ((yi + dy) % hubbard.ll);
                    const Eigen::VectorXd r = ( Eigen::VectorXd(2) << dx, dy ).finished();
                    this->matsubara_greens.tmp_value()(t) += hubbard.config_sign * cos(-r.dot(this->q)) * gt0(j, i) / hubbard.ls;
                }
            }
        }
    }
}

void Measure::DynamicMeasure::measure_density_of_states(const int &t, const Model::Hubbard &hubbard) {
    assert( t >= 0 && t < hubbard.lt );
    // spin degenerate model
    const Eigen::MatrixXd gt0 = ( t == 0 )?
              0.5 * (hubbard.vec_green_tt_up[hubbard.lt-1] + hubbard.vec_green_tt_up[hubbard.lt-1])
            : 0.5 * (hubbard.vec_green_t0_up[t-1] + hubbard.vec_green_t0_up[t-1]);
    this->density_of_states.tmp_value()(t) += hubbard.config_sign * gt0.trace() / hubbard.ls;
}

void Measure::DynamicMeasure::measure_superfluid_stiffness(const Model::Hubbard &hubbard) {
    // momentum qx and qy
    const Eigen::VectorXd qx = ( Eigen::VectorXd(2) << 2 * M_PI / hubbard.ll, 0.0 ).finished();
    const Eigen::VectorXd qy = ( Eigen::VectorXd(2) << 0.0, 2 * M_PI / hubbard.ll ).finished();
    
    // Superfluid stiffness \rho_s = 1/4 * ( Gamma^L - Gamma^T )
    double tmp_rho_s = 0.0;
    Eigen::MatrixXd gt0_up, g0t_up, gtt_up;
    Eigen::MatrixXd gt0_dn, g0t_dn, gtt_dn;
    const Eigen::MatrixXd g00_up = hubbard.vec_green_tt_up[hubbard.lt-1]; 
    const Eigen::MatrixXd g00_dn = hubbard.vec_green_tt_dn[hubbard.lt-1]; 

    for (int l = 0; l < hubbard.lt; ++l) {
        const int tau = (l == 0)? hubbard.lt-1 : l-1;
        gt0_up = hubbard.vec_green_t0_up[tau];
        g0t_up = hubbard.vec_green_0t_up[tau];
        gtt_up = hubbard.vec_green_tt_up[tau];
        gt0_dn = hubbard.vec_green_t0_dn[tau];
        g0t_dn = hubbard.vec_green_0t_dn[tau];
        gtt_dn = hubbard.vec_green_tt_dn[tau];

        // space point i is chosen as our base point, which is going to be averaged
        for (int xi = 0; xi < hubbard.ll; ++xi) {
            for (int yi = 0; yi < hubbard.ll; ++yi) {
                const int i = xi + hubbard.ll * yi;
                const int ipx = (xi + 1) % hubbard.ll + hubbard.ll * yi;

                // displacement
                for (int dx = 0; dx < hubbard.ll; ++dx) {
                    for (int dy = 0; dy < hubbard.ll; ++dy) {
                        /* for a given site l and time-slice tau
                         * the current-current correlation Jx-Jx: \Gamma_xx (l, \tau) = < jx(l, \tau) * jx(0, 0) > */
                        const int j = (xi + dx) % hubbard.ll + hubbard.ll * ((yi + dy) % hubbard.ll);
                        const int jpx = (xi + dx + 1) % hubbard.ll + hubbard.ll * ((yi + dy) % hubbard.ll);
                        const Eigen::VectorXd r = ( Eigen::VectorXd(2) << dx, dy ).finished();
                        const double factor = hubbard.config_sign * (cos(r.dot(qx)) - cos(r.dot(qy)));

                        tmp_rho_s += hubbard.t * hubbard.t * factor * (
                                // uncorrelated part
                                - ( gtt_up(j, jpx) - gtt_up(jpx, j) + gtt_dn(j, jpx) - gtt_dn(jpx, j) ) *
                                  ( g00_up(i, ipx) - g00_up(ipx, i) + g00_dn(i, ipx) - g00_dn(ipx, i) )
                                
                                // correlated part
                                - g0t_up(ipx, jpx) * gt0_up(j, i) - g0t_dn(ipx, jpx) * gt0_dn(j, i)
                                + g0t_up(i, jpx) * gt0_up(j, ipx) + g0t_dn(i, jpx) * gt0_dn(j, ipx)
                                + g0t_up(ipx, j) * gt0_up(jpx, i) + g0t_dn(ipx, j) * gt0_dn(jpx, i)
                                - g0t_up(i, j) * gt0_up(jpx, ipx) - g0t_dn(i, j) * gt0_dn(jpx, ipx) );
                    }
                }
            }
        }
    }
    // average over base point i
    // the 1/4 prefactor is due to Cooper pairs with charge 2
    // see https://arxiv.org/pdf/1912.08848.pdf
    this->superfluid_stiffness.tmp_value() += 0.25 * tmp_rho_s / hubbard.ls / hubbard.ls;
}

void Measure::DynamicMeasure::analyse_stats(const Model::Hubbard &hubbard) {
    this->sign.analyse();
    this->matsubara_greens.analyse();
    this->density_of_states.analyse();
    this->superfluid_stiffness.analyse();
    // this->current_current_corr.analyse();
}
