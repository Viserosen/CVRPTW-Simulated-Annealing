#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <string>
#include <sstream>
#include <cfloat>
#include <iomanip>
#include <ctime>

using namespace std;

struct Customer
{
    int id;
    int x;
    int y;
    int demand;
    int due_time;
    int ready_time;
    int service_time;
    int visited;
};
struct Depot
{
    int x;
    int y;
    int due_time;
};
struct Route
{
    vector<int> customers;
    int capacity;
    double cost;
};

// global variables
int solution_time = 300; // max time of searching for solution
clock_t start, interval;
int vehicle_capacity;
int trucks = 0;
double cost = 0;
double best_cost = 0;
vector<Route> all_routes;
vector<Route> best_routes;
vector<Customer> all_customers;
Depot depot;

void data_read(string input)
{
    string line;
    int a, b;
    ifstream file;
    file.open(input);
    getline(file, line);
    getline(file, line);
    getline(file, line);
    getline(file, line);

    file >> a >> vehicle_capacity;
    getline(file, line);
    getline(file, line);
    getline(file, line);
    bool depot_read = false;
    getline(file, line);
    while (getline(file, line) && !file.eof())
    {
        if (line.size() < 3 || line.empty())
            continue;

        istringstream ss(line);
        Customer temp;
        ss >> temp.id >> temp.x >> temp.y >> temp.demand >> temp.ready_time >> temp.due_time >> temp.service_time;
        temp.visited = 0;
        if (!depot_read)
        {
            depot.x = temp.x;
            depot.y = temp.y;
            depot.due_time = temp.due_time;
            depot_read = true;
            continue;
        }
        all_customers.push_back(temp);
    }
    // all_customers.pop_back();
    file.close();
}

void data_print()
{
    cout << "depot:\n";
    cout << depot.x << " " << depot.y << " " << depot.due_time << endl;
    cout << "customers:\n";
    for (const auto &customer : all_customers)
    {
        std::cout << customer.id << " " << customer.x << " " << customer.y << " "
                  << customer.demand << " " << customer.ready_time << " " << customer.due_time << " "
                  << customer.service_time << "\n";
    }
}

double distance_calculate(int x, int y, int x1, int y1)
{
    double distance = sqrt(pow(x - x1, 2) + pow(y - y1, 2));
    return distance;
}

bool all_customers_finished()
{
    for (int i = 0; i < all_customers.size(); i++)
    {
        if (!all_customers[i].visited)
        {
            return false;
        }
    }
    return true;
}

void greed_cvrptw()
{
    double time = 0;
    double min_time, temp_time;
    int x, y, next_customer, capacity_left;
    Route route;
    route.capacity = vehicle_capacity;
    while (1)
    {
        all_routes.push_back(route);
        capacity_left = vehicle_capacity;
        time = 0;
        x = depot.x;
        y = depot.y;

        while (1)
        {
            next_customer = -1;
            min_time = DBL_MAX;

            for (int i = 0; i < all_customers.size(); i++)
            {
                if (!all_customers[i].visited && all_customers[i].demand <= capacity_left)
                {
                    temp_time = distance_calculate(x, y, all_customers[i].x, all_customers[i].y);
                    temp_time = fmax(time + temp_time, all_customers[i].ready_time);

                    if (temp_time <= all_customers[i].due_time)
                    {
                        if (temp_time + all_customers[i].service_time + distance_calculate(all_customers[i].x, all_customers[i].y, depot.x, depot.y) < depot.due_time)
                        {
                            if (temp_time < min_time)
                            {
                                min_time = temp_time;
                                next_customer = i;
                            }
                        }
                    }
                }
            }
            if (next_customer == -1)
            {
                cost += distance_calculate(x, y, depot.x, depot.y) + time;
                all_routes.back().cost = distance_calculate(x, y, depot.x, depot.y) + time;
                break;
            }

            all_customers.at(next_customer).visited = 1;
            capacity_left -= all_customers[next_customer].demand;
            time = min_time + all_customers[next_customer].service_time;
            x = all_customers[next_customer].x;
            y = all_customers[next_customer].y;
            all_routes.back().customers.push_back(all_customers[next_customer].id);

            if (capacity_left == 0)
            {
                cost += distance_calculate(x, y, depot.x, depot.y) + time;
                break;
            }
        }

        all_routes.back().capacity = capacity_left;

        if (all_routes.back().customers.size() == 0)
        {
            trucks = -1;
            all_routes.pop_back();
            break;
        }

        if (all_customers_finished())
        {
            trucks = all_routes.size();
            break;
        }
    }
}

bool route_check(vector<Route> copy_all_route, int a)
{
    int x = depot.x;
    int y = depot.y;
    double time = 0;
    int capacity = 0;
    for (int i = 0; i < copy_all_route[a].customers.size(); i++)
    {
        capacity += all_customers[copy_all_route[a].customers[i] - 1].demand;
        time += distance_calculate(x, y, all_customers[copy_all_route[a].customers[i] - 1].x, all_customers[copy_all_route[a].customers[i] - 1].y);
        if (time > all_customers[copy_all_route[a].customers[i] - 1].due_time)
        {
            return false;
        }
        time = fmax(time, all_customers[copy_all_route[a].customers[i] - 1].ready_time);
        time += all_customers[copy_all_route[a].customers[i] - 1].service_time;
        x = all_customers[copy_all_route[a].customers[i] - 1].x;
        y = all_customers[copy_all_route[a].customers[i] - 1].y;
    }
    time += distance_calculate(x, y, depot.x, depot.y);
    if (time > depot.due_time)
    {
        return false;
    }
    if (capacity > vehicle_capacity)
    {
        return false;
    }

    return true;
}

bool time_check()
{
    interval = clock();
    float sec = (float)(interval - start) / CLOCKS_PER_SEC;
    if (sec > solution_time)
    {
        return false;
    }
    return true;
}

double recalculate_cost(vector<Route> new_routes)
{
    double new_cost = 0;
    for (int i = 0; i < new_routes.size(); i++)
    {
        new_cost += new_routes[i].cost;
    }
    return new_cost;
}
double calculate_cost(vector<Route> copy_all_route)
{
    int x = depot.x;
    int y = depot.y;
    double time = 0;
    double total = 0;
    for (int i = 0; i < copy_all_route.size(); i++)
    {
        x = depot.x;
        y = depot.y;
        time = 0;
        for (int k = 0; k < copy_all_route[i].customers.size(); k++)
        {
            time += distance_calculate(x, y, all_customers[copy_all_route[i].customers[k] - 1].x, all_customers[copy_all_route[i].customers[k] - 1].y);
            time = fmax(time, all_customers[copy_all_route[i].customers[k] - 1].ready_time) + all_customers[copy_all_route[i].customers[k] - 1].service_time;
            x = all_customers[copy_all_route[i].customers[k] - 1].x;
            y = all_customers[copy_all_route[i].customers[k] - 1].y;
        }
        time += distance_calculate(x, y, depot.x, depot.y);
        total += time;
    }
    return total;
}

void simulated_annealing()
{
    vector<Route> copy_all_routes = all_routes;

    int a, b, c, d, temp;
    double new_cost, new_route_cost, E;
    best_cost = cost;
    best_routes = all_routes;

    srand(time(NULL));

    // parameters of simulated annealing
    double Tmax, Tmin, T;
    Tmax = 200;
    T = Tmax;
    Tmin = 0.00001;
    double Cooling_factor = 0.99995;

    start = clock();
    while (T > Tmin && time_check())
    {
        copy_all_routes = all_routes;
        // choosing type of swap in routes
        if (rand() % 2)
        {
            do
            {
                a = rand() % copy_all_routes.size();
                if (!time_check)
                    return;
            } while (copy_all_routes[a].customers.size() < 2);

            b = rand() % copy_all_routes[a].customers.size();

            do
            {
                c = rand() % copy_all_routes[a].customers.size();
                if (!time_check)
                    return;
            } while (c == b);

            temp = copy_all_routes[a].customers[b];
            copy_all_routes[a].customers[b] = copy_all_routes[a].customers[c];
            copy_all_routes[a].customers[c] = temp;

            if (route_check(copy_all_routes, a))
            {
                new_cost = calculate_cost(copy_all_routes);
            }
            else
            {
                continue;
            }
        }
        else
        {
            do
            {
                a = rand() % copy_all_routes.size();
                b = rand() % copy_all_routes.size();
                if (!time_check)
                    return;
            } while (a == b);

            c = rand() % copy_all_routes[a].customers.size();
            d = rand() % copy_all_routes[b].customers.size();

            temp = copy_all_routes[a].customers[c];
            copy_all_routes[a].customers[c] = copy_all_routes[b].customers[d];
            copy_all_routes[b].customers[d] = temp;

            if (route_check(copy_all_routes, a) && route_check(copy_all_routes, b))
            {
                new_cost = calculate_cost(copy_all_routes);
            }
            else
            {
                continue;
            }
        }
        E = cost - new_cost;
        if (best_cost > new_cost)
        {
            best_cost = new_cost;
            best_routes = copy_all_routes;
        }

        if (E > 0)
        {
            cost = new_cost;
            all_routes = copy_all_routes;
        }
        else if (exp(E / T) > rand() % 2)
        {
            cost = new_cost;
            all_routes = copy_all_routes;
        }
        T = T * Cooling_factor;
        // cout << T << endl;
    }
}

void output_file(string output)
{
    ofstream file_out;
    file_out.open(output);
    if (trucks == -1)
    {
        file_out << "-1";
        file_out.close();
        return;
    }
    file_out << trucks << " " << fixed << setprecision(5) << cost << endl;
    for (int i = 0; i < all_routes.size(); i++)
    {
        for (int k = 0; k < all_routes[i].customers.size(); k++)
        {
            file_out << all_routes[i].customers[k] << " ";
        }
        file_out << endl;
    }

    file_out.close();
}

int main(int argc, char *argv[])
{
    string inputfile, outputfile;
    if (argc >= 2)
    {
        inputfile = argv[1];
        outputfile = argv[2];
        if (argc >= 3)
        {
            solution_time = atoi(argv[3]);
        }
    }
    else
    {
        inputfile = "data.txt";
        outputfile = "solution.txt";
    }

    data_read(inputfile);
    cout << depot.x << " " << depot.y << " " << depot.due_time;
    // data_print();
    greed_cvrptw();
    if (trucks != -1)
    {
        simulated_annealing();
        all_routes = best_routes;
        cost = best_cost;
    }

    output_file(outputfile);
    return 0;
}