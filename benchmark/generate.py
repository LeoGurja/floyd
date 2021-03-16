from random import randint
import sys


def create_random_matrix():
    n = int(sys.argv[1])
    filename = sys.argv[2]
    content = ""
    for i in range(n):
        for j in range(n):
            if (i == j):
                content += "0 "
            else:
                number = randint(1, 11)
                if (number == 11):
                    number = 99999
                content += str(number) + " "
        if (i != n - 1):
            content += "\n"
    with open(filename, 'w') as file:
        file.write(content)


create_random_matrix()
