import os.path
import subprocess
block_sizes = [
    '1000', '2000', '3000', '4000', '5000', '6000', '7000', '8000'
    ]

'''
Return a list containing all the digits extracted from a string
'''
def extract_digits(str):
    return [float(s) for s in str.split() if '.' in s]


def experiment_csv2colstore(block_size, f):

    total = 0
    print("Page size: {}".format(block_size))

    for i in range(10):
        # run one time for each block size
        output = subprocess.check_output(["./csv2colstore", "sample.csv", "col_store", block_size])

        # Decode the byte-string and extract all the numeric values
        output = output.decode("utf-8")
        time = extract_digits(output)[0]
        total += time

    
    avg_rate = total / 10

    print("Time Consumed: {}".format(avg_rate))
    f.write("{} {}\r\n".format(block_size, avg_rate))


def experiment_select2(block_size, f):

    total = 0
    print("Page size: {}".format(block_size))
    # For each block size, run the program 100 times and record the average
    for i in range(100):
        output = subprocess.check_output(["./select2", "col_store", "0", "A", "D", block_size])

        # Decode the byte-string and extract all the numeric values
        output = output.decode("utf-8")
        time = extract_digits(output)[0]
        total += time

    avg_rate = total / 100
    print("Rate: {}".format(avg_rate))
    f.write("{} {}\r\n".format(block_size, avg_rate))

def experiment_select3(block_size, f):

    total = 0
    print("Page size: {}".format(block_size))
    # For each block size, run the program 100 times and record the average
    for i in range(100):
        output = subprocess.check_output(["./select3", "col_store", "0", "1", "A", "D", block_size])

        # Decode the byte-string and extract all the numeric values
        output = output.decode("utf-8")
        time = extract_digits(output)[0]

        total += time

    avg_rate = total / 100
    print("Rate: {}".format(avg_rate))
    f.write("{} {}\r\n".format(block_size, avg_rate))

def main():
    
    # csv2colstore    
    f = open("csv2colstore_data.txt", "w")
    f.write("block_size time\r\n")

    for block_size in block_sizes:
            experiment_csv2colstore(block_size, f)

    f.close()

    # select2
    f = open("select2_data.txt","w")
    f.write("block_size time\r\n")
    for block_size in block_sizes:
        experiment_select2(block_size, f)

    f.close()
    
    # select3
    f = open("select3_data.txt","w")
    f.write("block_size time\r\n")
    for block_size in block_sizes:
        experiment_select3(block_size, f)

    f.close()


if __name__ == "__main__":
    main()