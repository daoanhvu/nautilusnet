import configparser as cp

def process_configure(config_file):
    common_params = {}
    dataset_params = {}
    net_params = {}
    solver_params {}

    parser = cp.ConfigParser()
    parser.read(config_file)

    for sect in parser.sections():
        print(sect)

    return common_params, dataset_params, net_params, solver_params
