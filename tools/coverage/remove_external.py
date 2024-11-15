if __name__ == "__main__":
    skip = False
    filename = "bazel-out/_coverage/_coverage_report.dat"
    with open(filename, 'r') as file:
        out_file = open("bazel-out/_coverage/coverage_report.dat", 'w')
        for line in file.readlines():
            if line.startswith("SF:") and "external/" in line:
                skip = True
            if "end_of_record" in line:
                skip = False
            if not skip:
                out_file.write(line)
        out_file.close()