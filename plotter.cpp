#include "plotter.h"

Plotter::Plotter(const Controller& controller)
	: controller_(std::make_unique<Controller>(controller))
{
}

void Plotter::plotComparativeMolarVolumes(double pressure_start, double pressure_end, double pressure_inc, std::array<double, 3> fixed_pressures, std::pair<size_t, size_t> indecies)
{
    auto data = controller_->getData();
    auto gas_name = data.names[indecies.first];
    auto liquid_name = data.names[indecies.second];
    auto gas_state = data.states.at(gas_name);
    auto liquid_state = data.states.at(liquid_name);

    controller_->setConcentrationState(indecies.first);
    auto gas_calculation = controller_->calculateVolumeInRange(Controller::generateJsonName(gas_state, RequestType::EXACT_MOLAR), pressure_start, pressure_end, pressure_inc);

    controller_->setConcentrationState(indecies.second);
    auto liquid_calculation = controller_->calculateVolumeInRange(Controller::generateJsonName(liquid_state, RequestType::EXACT_MOLAR), pressure_start, pressure_end, pressure_inc);

    auto approximation = controller_->approximateVolumeInRange(Controller::generateJsonName(gas_state, RequestType::APPROXIMATION_MOLAR), pressure_start, pressure_end, pressure_inc, fixed_pressures, indecies);

    plotTwoVectors(generatePNGName("molar_gas", gas_state.temperature), gas_calculation.pressures, gas_calculation.volumes, approximation.gas.pressures, approximation.gas.volumes);
    plotTwoVectors(generatePNGName("molar_liquid", liquid_state.temperature), liquid_calculation.pressures, liquid_calculation.volumes, approximation.liquid.pressures, approximation.liquid.volumes);
}

void Plotter::plotComparativeSpecificVolumes(double pressure_start, double pressure_end, double pressure_inc, std::array<double, 3> fixed_pressures, std::pair<size_t, size_t> indecies)
{
    auto data = controller_->getData();
    auto gas_name = data.names[indecies.first];
    auto liquid_name = data.names[indecies.second];
    auto gas_state = data.states.at(gas_name);
    auto liquid_state = data.states.at(liquid_name);

    controller_->setConcentrationState(indecies.first);
    auto gas_calculation = controller_->calculateSpecificVolumeInRange(Controller::generateJsonName(gas_state, RequestType::EXACT_SPECIFIC), pressure_start, pressure_end, pressure_inc);

    controller_->setConcentrationState(indecies.second);
    auto liquid_calculation = controller_->calculateSpecificVolumeInRange(Controller::generateJsonName(liquid_state, RequestType::EXACT_SPECIFIC), pressure_start, pressure_end, pressure_inc);

    auto approximation = controller_->approximateSpecificVolumeInRange(Controller::generateJsonName(gas_state, RequestType::APPROXIMATION_SPECIFIC), pressure_start, pressure_end, pressure_inc, fixed_pressures, indecies);

    plotTwoVectors(generatePNGName("specific_gas", gas_state.temperature), gas_calculation.pressures, gas_calculation.volumes, approximation.gas.pressures, approximation.gas.volumes);
    plotTwoVectors(generatePNGName("specifi_liquid", liquid_state.temperature), liquid_calculation.pressures, liquid_calculation.volumes, approximation.liquid.pressures, approximation.liquid.volumes);
}

void Plotter::plotComparativeKValues(double pressure_start, double pressure_end, double pressure_inc, std::array<double, 3> fixed_pressures, std::pair<size_t, size_t> indecies)
{
    auto data = controller_->getData();
    auto gas_name = data.names[indecies.first];
    auto gas_state = data.states.at(gas_name);

    controller_->setConcentrationState(indecies.first);
    auto def_kvalues = controller_->calculateKValuesByDefinitionInRange(Controller::generateJsonName(gas_state, RequestType::EXACT_KVALUE), pressure_start, pressure_end, pressure_inc, indecies);
    auto wilson_kvalues = controller_->calculateKValuesByWilsonInRange(Controller::generateJsonName(gas_state, RequestType::WILSON_KVALUE), pressure_start, pressure_end, pressure_inc);
    auto app_kvalues = controller_->approximateKValuesInRange(Controller::generateJsonName(gas_state, RequestType::APPROXIMATION_KVALUE), pressure_start, pressure_end, pressure_inc, fixed_pressures, indecies);

    for (const auto& component : controller_->getData().components)
    {
        plotThreeVectors(generatePNGName(component.name, gas_state.temperature), generatePNGName(component.name, gas_state.temperature), def_kvalues.pressures, def_kvalues.kvalues.at(component.name), wilson_kvalues.pressures, wilson_kvalues.kvalues.at(component.name), app_kvalues.pressures, app_kvalues.kvalues.at(component.name));
    }

    plotAllVectors(generatePNGName("special", gas_state.temperature), app_kvalues.pressures, app_kvalues.kvalues);
}

void Plotter::plotTwoVectors(const std::string& filename,  const std::vector<double>& x1, const std::vector<double>& y1, const std::vector<double>& x2, const std::vector<double>& y2)
{
    try
    {
        Gnuplot gp("\"D:\\gnuplot\\bin\\gnuplot.exe\"");

        gp << "set terminal pngcairo size 1280,960 enhanced font 'Arial,14'\n";

        gp << "set output '" << filename << "'\n";

        gp << "set xlabel font ',18' 'P'\n";
        gp << "set ylabel font ',18' 'V'\n";

        gp << "set xtics font ',16'\n";
        gp << "set ytics font ',16'\n";

        gp << "set grid\n";

        gp << "plot '-' with lines lc 'black' lw 1 dt 1 title 'Exact', "
            << "'-' with lines lc 'black' lw 3 dt 2 title 'Approximation'\n";

        gp.send1d(std::make_tuple(x1, y1));
        gp.send1d(std::make_tuple(x2, y2));

        gp << "pause mouse close\n";
    }
    catch (const std::exception& e)
    {
        std::cerr << "Ошибка: " << e.what() << std::endl;
    }
}

void Plotter::plotThreeVectors(const std::string& title, const std::string& filename, const std::vector<double>& x1, const std::vector<double>& y1, const std::vector<double>& x2, const std::vector<double>& y2, const std::vector<double>& x3, const std::vector<double>& y3)
{
    try
    {
        Gnuplot gp("\"D:\\gnuplot\\bin\\gnuplot.exe\"");

        gp << "set terminal pngcairo size 1280,960 enhanced font 'Arial,14'\n";

        gp << "set output '" << filename << "'\n";

        gp << "set title '" << title << "'\n";
        
        gp << "set xlabel font ',18' 'P'\n";
        gp << "set ylabel font ',18' 'KValue'\n";

        gp << "set xtics font ',16'\n";
        gp << "set ytics font ',16'\n";

        gp << "set grid\n";

        gp << "plot '-' with lines lc 'black' lw 1 dt 1 title 'Exact', "
            << "'-' with lines lc 'black' lw 2 dt 3 title 'Wilson', "
            << "'-' with lines lc 'black' lw 3 dt 2 title 'Approximation'\n";

        gp.send1d(std::make_tuple(x1, y1));
        gp.send1d(std::make_tuple(x2, y2));
        gp.send1d(std::make_tuple(x3, y3));

        gp << "pause mouse close\n";
    }
    catch (const std::exception& e)
    {
        std::cerr << "Ошибка: " << e.what() << std::endl;
    }
}

void Plotter::plotAllVectors(const std::string& title, const std::vector<double>& pressures, const std::unordered_map<std::string, std::vector<double>> k_values)
{
    try
    {
        Gnuplot gp("\"D:\\gnuplot\\bin\\gnuplot.exe\"");

        // Уберите расширение из названия файла, если оно есть
        std::string filename = title;
        if (filename.size() < 4 || filename.substr(filename.size() - 4) != ".png") {
            filename += ".png";
        }

        gp << "set terminal pngcairo size 1280,960 enhanced font 'Arial,14'\n";
        gp << "set output '" << filename << "'\n";
        gp << "set logscale xy\n";  // Логарифмические шкалы по обеим осям
        gp << "set format x '10^{%L}'\n";
        gp << "set format y '10^{%L}'\n";
        gp << "set xlabel font ',18' 'P'\n";
        gp << "set ylabel font ',18' 'KValue'\n";
        gp << "set xtics font ',16'\n";
        gp << "set ytics font ',16'\n";
        gp << "set grid\n";
        gp << "set key font ',14'\n";

        // Строим одну команду plot
        std::stringstream plotCmd;
        plotCmd << "plot ";

        bool first = true;
        for (const auto& kvalue : k_values)
        {
            if (!first) {
                plotCmd << ", ";
            }
            plotCmd << "'-' with lines lw 2 title '" << kvalue.first << "'";
            first = false;
        }
        plotCmd << "\n";

        gp << plotCmd.str();

        for (const auto& kvalue : k_values)
        {
            if (pressures.size() != kvalue.second.size()) {
                std::cerr << "Ошибка: размеры векторов не совпадают для " << kvalue.first
                    << " (" << pressures.size() << " vs " << kvalue.second.size() << ")\n";
                continue;
            }

            std::vector<std::pair<double, double>> data;
            data.reserve(pressures.size());
            for (size_t i = 0; i < pressures.size(); ++i) {
                data.emplace_back(pressures[i], kvalue.second[i]);
            }
            gp.send1d(data);
        }

    }
    catch (const std::exception& e)
    {
        std::cerr << "Ошибка: " << e.what() << std::endl;
    }
}

std::string Plotter::generatePNGName(const std::string& name, double temperature)
{
    std::stringstream ss;

    ss << name << "-" << temperature;
    ss << ".png";

    return ss.str();
}

