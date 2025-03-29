# CVRPTW-Simulated-Annealing

Metaheuristic to solve CVRPTW for files of Solomon’s Instances.

### CVRPTW

The Capacitated Vehicle Routing Problem with Time Windows (CVRPTW) is an extension of the classical Vehicle Routing Problem (VRP). In CVRPTW, a fleet of vehicles with limited capacity must deliver goods to a set of customers while respecting predefined time windows for each delivery. The objective is to minimize the total travel cost, which can include distance or time, while ensuring that each customer is served within their specified time window.

### Simulated Annealing

Simulated Annealing (SA) is a probabilistic optimization technique inspired by the annealing process in metallurgy. It is used to find approximate solutions to complex optimization problems, including CVRPTW. SA starts with an initial solution and iteratively explores neighboring solutions, accepting worse solutions with a certain probability to escape local optima. The probability of accepting worse solutions decreases over time, controlled by a cooling schedule. This allows SA to effectively balance exploration and exploitation.

### Using this solver

##### Compilation

To compile the program, use the following command:

    g++ -o solver SACVRPTWsolution.cpp

##### Running the Program

Execute the program from the command line with the required parameters:

    ./solver <input_file_name> <output_file_name> <solution_search_max_time>

If the program is run without parameters, the default values will be used. - input_file_name: data.txt - output_file_name: solution.txt - solution_search_max_time: 300
Solution max time is in seconds increase the parameter for potentially better results. The output file will contain the best-found solution within the given time.

### Input and output file structure

    i: customer number
    x: x coordinate
    y: y coordinate
    q: demand
    e: ready time
    l: due date
    d: service duration

##### Input:

    Problem_name\n
    \n
    VEHICLE\n
    NUMBER CAPACITY\n
    K Q\n
    \n
    CUSTOMER\n
    CUST NO. XCOORD. YCOORD. DEMAND READY TIME DUE DATE SERVICE TIME\n
    i_0 x_0 y_0 q_0 e_0 l_0 d_0\n
    ...
    i_n x_n y_n q_n e_n l_n d_n\n

##### Output:

    Number_of_routes Cost/distance_of_routes\n
    first_customer_of_first_route ... last_customer_of_first_route\n
    ...
    first_customer_of_last_route ... last_customer_of_last_route\n
