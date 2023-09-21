#include <ilcplex/ilocplex.h>
#include <iostream>

int helloCPLEX() {
    IloEnv env;
    try {
        // Create an instance of the CPLEX model.
        IloModel model(env);

        // Create variables: x and y
        IloNumVar x(env, 0, IloInfinity, ILOFLOAT);
        IloNumVar y(env, 0, IloInfinity, ILOFLOAT);

        // Add variables to the model
        model.add(x);
        model.add(y);

        // Create the objective function: maximize 3x + 4y
        IloObjective obj = IloMaximize(env, 3 * x + 4 * y);
        model.add(obj);

        // Add constraints
        model.add(2 * x + y <= 5);
        model.add(x + 3 * y <= 12);

        // Create the CPLEX solver
        IloCplex cplex(model);

        // Set solver parameters (optional)
        // cplex.setParam(IloCplex::Param::TimeLimit, 10);

        // Solve the LP
        cplex.solve();

        // Display solution status and optimal values
        if (cplex.getStatus() == IloAlgorithm::Optimal) {
            std::cout << "Solution status: Optimal" << std::endl;
            std::cout << "Objective value: " << cplex.getObjValue() << std::endl;
            std::cout << "x = " << cplex.getValue(x) << std::endl;
            std::cout << "y = " << cplex.getValue(y) << std::endl;
        } else {
            std::cerr << "No optimal solution found." << std::endl;
        }
    } catch (IloException& e) {
        std::cerr << "Error: " << e.getMessage() << std::endl;
    }

    env.end();

    return 0;
}