#include "Hubbard.h"
#include "DetQMC.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <unistd.h>
#include <boost/program_options.hpp>
#include <boost/format.hpp>

/**
  *  TODO:
  *   1. get params from command lines, using boost (done)
  *   2. equal-time measurements of momentum distribution and spin-spin correlation (done)
  *   3. bin measurements (done)
  *   4. time-displaced green function and measurements (done)
  *   5. ******** Modify command console output ******** (done)
  *   6. attractive interaction U < 0 (done)
  *   7. determine the critical temperature of superconducting transition (done)
  *   8. reweighing for doped case (done)
  *   9. read aux field configurations from input file (done)
  *   10. checkerboard decomposition (done)
  *   11. openmp parallel sampling (missing)
  *   12. replace boost::format with std::format, using c++20 standard (missing)
  *   13. log output (missing)
  *   14. simulate with bash script (missing)
  *   15. ...
  */


/**  The Main Program */
int main(int argc, char* argv[]) {

    /** model and controlling params */
    int ll = 4;
    int lt = 80;
    double beta = 4.0;
    double t = 1.0;
    double u = -4.0;
    double mu = 0.0;
    bool bool_checkerboard = false;

    int nwrap = 10;
    int nwarm = (int)(4 * ll * ll * beta);

    int nbin = 20;
    int nsweep = 100;
    int nBetweenBins = 10;

    std::string filename_eqtime = "../results/meas-eqtime.dat";
    std::string filename_dynamic = "../results/meas-dynamic.dat";
    bool bool_display_process = true;

    bool bool_warm_up = true;
    bool bool_measure_eqtime = true;
    bool bool_measure_dynamic = true;


    /** read params from command line */
    boost::program_options::options_description opts("Program options");
    boost::program_options::variables_map vm;

    opts.add_options()
            ("help,h", "display this information")
            ("ll", boost::program_options::value<int>(&ll)->default_value(4), "spatial size of lattice, default: 4")
            ("lt", boost::program_options::value<int>(&lt)->default_value(80), "imaginary-time size of lattice, default: 80")
            ("beta", boost::program_options::value<double>(&beta)->default_value(4.0), "inverse temperature, default: 4.0")
            ("t", boost::program_options::value<double>(&t)->default_value(1.0), "hopping strength, default: 1.0")
            ("u", boost::program_options::value<double>(&u)->default_value(-4.0),
                    "interaction strength, u > 0 for repulsive and u < 0 for attractive case, default: -4.0")
            ("mu", boost::program_options::value<double>(&mu)->default_value(0.0), "chemical potential, default: 0.0")
            ("checkerboard", boost::program_options::value<bool>(&bool_checkerboard)->default_value(false), "whether to perform checkerboard break-up, default: false")
            ("nwrap", boost::program_options::value<int>(&nwrap)->default_value(10), "pace of stabilization process, default: 10")
            ("nwarm", boost::program_options::value<int>(&nwarm)->default_value((int)(4*ll*ll*beta)), "number of warmup sweeps, default: 4*ll*ll*beta")
            ("nbin", boost::program_options::value<int>(&nbin)->default_value(20), "number of bins, default: 20")
            ("nsweep", boost::program_options::value<int>(&nsweep)->default_value(100), "number of measurement sweeps in a bin, default: 100")
            ("nbetweenbins", boost::program_options::value<int>(&nBetweenBins)->default_value(10),
                    "number of sweeps between bins to avoid correlation, default: 10")
            ("eqtime", boost::program_options::value<bool>(&bool_measure_eqtime)->default_value(true), "whether to do equal-time measurements, default: true")
            ("dynamic", boost::program_options::value<bool>(&bool_measure_dynamic)->default_value(true), "whether to do dynamic measurements, default: true")
            ("oeq", boost::program_options::value<std::string>(&filename_eqtime)->default_value("../results/meas-eqtime.dat"),
                    "output filename of equal-time data, default: ../results/meas-eqtime.dat")
            ("ody", boost::program_options::value<std::string>(&filename_dynamic)->default_value("../results/meas-dynamic.dat"),
                    "output filename of dynamic data, default: ../results/meas-dynamic.dat");

    try {
        boost::program_options::store(parse_command_line(argc, argv, opts), vm);
    }
    catch (...) {
        std::cerr << "Got undefined options from command line! "<< std::endl;
        exit(1);
    }
    boost::program_options::notify(vm);

    if (vm.count("help")) {
        std::cerr << argv[0] << std::endl;
        std::cerr << opts << std::endl;
        exit(1);
    }

    if ((!vm["ll"].defaulted() || !vm["beta"].defaulted()) && vm["nwarm"].defaulted()) {
        nwarm = 4 * vm["ll"].as<int>() * vm["ll"].as<int>() * (int)vm["beta"].as<double>();
    }


    /** DQMC Simulation */
    Simulation::DetQMC *dqmc;
    dqmc = new Simulation::DetQMC();


    /** usage example */
//    dqmc->set_model_params(ll, lt, beta, t, u, mu, nwrap, bool_checkerboard);
//    dqmc->set_Monte_Carlo_params(nwarm, nbin, nsweep, nBetweenBins);
//    dqmc->set_controlling_params(bool_warm_up, bool_measure_eqtime, bool_measure_dynamic);
//    dqmc->set_lattice_momentum(1.0, 1.0);
//    dqmc->print_params();
//
//    dqmc->init_measure();
//    dqmc->run_QMC(bool_display_process);
//    dqmc->analyse_stats();
//    dqmc->print_stats();

    /** Measure dynamical correlation functions for different momentum k */

//    dqmc->set_model_params(ll, lt, beta, t, u, mu, nwrap, bool_checkerboard);
//    dqmc->set_Monte_Carlo_params(nwarm, nbin, nsweep, nBetweenBins);
//    dqmc->set_controlling_params(bool_warm_up, bool_measure_eqtime, bool_measure_dynamic);
//    dqmc->init_measure();            // init after params set
//    dqmc->print_params();
//    dqmc->run_QMC(bool_display_process);
//
//    std::vector<double> q_list = { 0.00, 0.25, 0.50, 0.75, 1.00, };
//    for (auto q : q_list ) {
////        dqmc->print_stats();
//        dqmc->set_lattice_momentum(q, q);
//        dqmc->analyse_stats();
//
//        /* creat folder and pack up data */
//        std::string path = (boost::format("../results/L%db%.2fU%.2fk%.2f%.2f") % ll % beta % u % q % q).str();
//        std::string command;
//        if ( access(path.c_str(), 0) != 0 ) {
//            command = "mkdir " + path;
//            if ( system(command.c_str()) != 0 ) {
//                std::cerr << "fail to create " + path << std::endl;
//            }
//        }
//
//        dqmc->file_output_tau(path + "/tau.dat");
//        dqmc->bin_output_corr(path + "/cor.dat");
//        dqmc->file_output_dynamic_stats(path + "/dynamic.dat");
//    }

    /** Measure local density of states (LDOS) **/

    for (int n = 0; n < 1; ++n) {
        dqmc->set_model_params(ll, lt, beta, t, u, mu, nwrap, bool_checkerboard);
        dqmc->set_Monte_Carlo_params(nwarm, nbin, nsweep, nBetweenBins);
        dqmc->set_controlling_params(bool_warm_up, bool_measure_eqtime, bool_measure_dynamic);
        dqmc->set_lattice_momentum(0.5, 0.5);
        dqmc->print_params();

        dqmc->init_measure();
        dqmc->run_QMC(bool_display_process);
        dqmc->analyse_stats();
        dqmc->print_stats();

        /* creat folder and pack up data */
        std::string path = (boost::format("../results/L%db%.2fU%.2f") % ll % beta % u).str();
        std::string command;
        if ( access(path.c_str(), 0) != 0 ) {
            command = "mkdir " + path;
            if ( system(command.c_str()) != 0 ) {
                std::cerr << "fail to create " + path << std::endl;
            }
        }
        dqmc->file_output_tau(path + "/tau.dat");
        dqmc->bin_output_LDOS(path + "/cor.dat");
        dqmc->file_output_dynamic_stats(path + "/dynamic.dat");
    }


    /** Measure observable quantities in momentum space ( fermi surface ) */

//    std::vector<double> list_u = { -4.0, -2.0, -1.0, };
//    for (auto uint : list_u) {
//        dqmc->set_model_params(ll, lt, beta, t, uint, mu, nwrap, bool_checkerboard);
//        dqmc->set_Monte_Carlo_params(nwarm, nbin, nsweep, nBetweenBins);
//        dqmc->set_controlling_params(true, false, true);
//        dqmc->init_measure();
//        dqmc->print_params();
//        dqmc->run_QMC(bool_display_process);
//
//        std::string filename = (boost::format("../results/FS_L%db%.2fU%.2f.dat") % ll % beta % u).str();
//        std::ofstream outfile;
//        outfile.open(filename, std::ios::out | std::ios::app);
//        for (int i = 0; i <= ll; ++i) {
//            // crystal momentum qx
//            const double qx = 1.0 - 2.0 * i / ll;
//
//            for (int j = 0; j <= ll - i; ++j) {
//                // time reverse symmetry: G(k, beta/2) = G(-k, beta/2)
//                // crystal momentum qy
//                const double qy = 1.0 - 2.0 * j / ll;
//
//                dqmc->set_lattice_momentum(qx, qy);
//                dqmc->analyse_stats();
//                dqmc->print_stats();
//
//                outfile << std::setiosflags(std::ios::right)
//                        << std::setw(15) << i
//                        << std::setw(15) << j
//                        << std::setw(15) << qx
//                        << std::setw(15) << qy
//                        << std::setw(15) << dqmc->DynamicMeasure.obs_mean_g_kt[ceil(lt/2)]
//                        << std::setw(15) << dqmc->DynamicMeasure.obs_err_g_kt[ceil(lt/2)]
//                        << std::endl;
//            }
//        }
//        outfile.close();
//    }


    /** Measure helicity modules over temperature T */

//    std::vector<double> list_beta = { 5.0, 5.0, 5.0, 8.0, };
//    for (auto Beta : list_beta) {
//        lt = (int)(Beta / 0.05);
//        dqmc->set_model_params(ll, lt, Beta, t, u, mu, nwrap, bool_checkerboard);
//        dqmc->set_Monte_Carlo_params(nwarm, nbin, nsweep, nBetweenBins);
//
//        const std::string file_configs = (boost::format("../results/rhos_L%du%.2f/config_lt%db%.2f.dat") % ll % u % lt % beta).str();
//        std::ifstream infile;
//        infile.open(file_configs, std::ios::in);
//        if (!infile.is_open()) {
//            std::cerr << "fail to open file " + file_configs + ", start simulation with random configs." << std::endl;
//            bool_warm_up = true;
//        }
//        else {
//            infile.close();
//            dqmc->read_aux_field_configs(file_configs);
//            std::cerr << "old configuration read from " + file_configs +", no need to warm up." << std::endl;
//            bool_warm_up = false;
//        }
//
//        dqmc->set_controlling_params(bool_warm_up, false, bool_measure_dynamic);
//        dqmc->set_lattice_momentum(1.0, 1.0);
//        dqmc->init_measure();
//        dqmc->print_params();
//
//        dqmc->run_QMC(bool_display_process);
//        dqmc->analyse_stats();
//        dqmc->print_stats();
//        dqmc->file_output_aux_field_configs(file_configs);
//
//        std::string filename = (boost::format("../results/rhos_L%dU%.2f/sc_rhos.dat") % ll % u).str();
//        std::string filename_bins = (boost::format("../results/rhos_L%dU%.2f/bins_b%.2f.dat") % ll % u % Beta).str();
//
//        std::ofstream outfile;
//        outfile.open(filename, std::ios::out | std::ios::app);
//        outfile << std::setiosflags(std::ios::right)
//                << std::setw(15) << Beta
//                << std::setw(15) << 1 / Beta
//                << std::setw(15) << dqmc->dynamicMeasure.obs_mean_rho_s
//                << std::setw(15) << dqmc->dynamicMeasure.obs_err_rho_s
//                << std::endl;
//        outfile.close();
//
//        outfile.open(filename_bins, std::ios::out | std::ios::app);
//        for (int bin = 0; bin < nbin; ++bin) {
//            outfile << std::setiosflags(std::ios::right)
//                    << std::setw(15) << bin + 1
//                    << std::setw(15) << dqmc->dynamicMeasure.obs_bin_rho_s[bin]
//                    << std::endl;
//        }
//        outfile.close();
//    }


    /** Checkerboard Benchmark */

//    std::chrono::steady_clock::time_point begin_t, end_t;
//
//    Model::Hubbard hubbard1(20, 80, 4.0, 1.0, 4.0, 0.0, 10, true);
//    Model::Hubbard hubbard2(20, 80, 4.0, 1.0, 4.0, 0.0, 10, false);
//
//    Eigen::MatrixXd test1 = Eigen::MatrixXd::Identity(hubbard1.ls, hubbard1.ls);
//    Eigen::MatrixXd test2 = Eigen::MatrixXd::Identity(hubbard2.ls, hubbard2.ls);
//
//    begin_t = std::chrono::steady_clock::now();
//    for (int i = 0; i < 100; ++i) {
//        hubbard1.mult_B_from_left(test1, 0, +1);
//    }
//    end_t = std::chrono::steady_clock::now();
//    std::cout << (double)std::chrono::duration_cast<std::chrono::milliseconds>(end_t - begin_t).count()/1000 << std::endl;
//
//    begin_t = std::chrono::steady_clock::now();
//    for (int i = 0; i < 100; ++i) {
//        hubbard2.mult_B_from_left(test2, 0, +1);
//    }
//    end_t = std::chrono::steady_clock::now();
//    std::cout << (double)std::chrono::duration_cast<std::chrono::milliseconds>(end_t - begin_t).count()/1000 << std::endl;

    delete dqmc;
    return 0;
}
