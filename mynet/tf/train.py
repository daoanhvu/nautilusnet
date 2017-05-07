import sys
sys.path.append('./')
import os
from tools.process_config import process_configure
from optparse import OptionParser
from net.yolo import YOLO
from dataset.text_dataset import TextDataSet
from solver.yolo_solver import YoloSolver

classes_name = ['teddy', 'ant', 'airplane']

def main():
    parser = OptionParser()
    parser.add_option("-c", "--conf", dest="configure", help="configure filename")
    (options, args) = parser.parse_args()
    print(options)
    if options.configure:
        config_file = str(options.configure)
        print(config_file)
    else:
        print("Please feed argument configure filename!")
        exit(0)

    common_params, dataset_params, net_params, solver_params = process_configure(config_file)
    print(common_params)
    print(dataset_params)
    ds = TextDataSet(common_params, dataset_params)
    net = YOLO(common_params, net_params)
    solver = YoloSolver(ds, net, common_params, solver_params)
    solver.solve(restore=True)

if __name__=="__main__":
    main()
