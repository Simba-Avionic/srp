# The code you provided is the main entry point of a Python script. It imports the `EndFileGenerator`
# class from the `utils.end_file_generator` module and the `argparse` module.

import argparse
import logging
from utils.end_file_generator import EndFileGenerator


if __name__ == "__main__":
    logging.basicConfig(level=logging.WARNING)

    parser=argparse.ArgumentParser(description="Config file generator for SOME/IP v1")

    parser.add_argument('service_name',type=str,help="Name service to generated")
    parser.add_argument('source_filepath',type=str,help="Path to source file")
    parser.add_argument('out_filepath',type=str,help="output file path (without filename)")
    args=parser.parse_args()

    generator = EndFileGenerator(args.service_name, args.source_filepath)
    generator.save_to_file(args.out_filepath)
