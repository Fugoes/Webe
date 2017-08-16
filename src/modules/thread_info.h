#include <string>
#include <vector>
#include <tuple>

std::vector<std::tuple<int, std::string, std::string>> get_threads_info();
std::tuple<int, int, int> get_running_sleeping_total_threads();
