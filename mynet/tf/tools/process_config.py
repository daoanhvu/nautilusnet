import configparser as cp

def process_configure(config_file):
    common_params = {}
    dataset_params = {}
    net_params = {}
    solver_params = {}

    parser = cp.ConfigParser()
    parser.read(config_file)

    #sections and options
    for section in parser.sections():
        #construct common_params
        if section == 'Common':
            for option in parser.options(section):
                common_params[option] = parser.get(section, option)
        #construct dataset_params
        if section == 'DataSet':
            for option in parser.options(section):
                dataset_params[option] = parser.get(section, option)
        #construct net_params
        if section == 'Net':
            for option in parser.options(section):
                net_params[option] = parser.get(section, option)
        #construct solver_params
        if section == 'Solver':
            for option in parser.options(section):
                solver_params[option] = parser.get(section, option)

    return common_params, dataset_params, net_params, solver_params
