# -*- coding: utf-8 -*-


import glob
import sys


def get_testcase_num():
    with open("run.sh") as fin:
        for line in fin.readlines():
            if "num_testcase=" in line:
                return int(line.split("=")[1].strip())


result_dir = "output"
num_testcase = get_testcase_num()
score_template = "{}/result_{}.txt"


def push_result():

    def get_score_testcase(seed):

        def get_test_path(seed):
            return "result/1/{}/stdout".format(seed)

        config = dict()
        with open(get_test_path(seed)) as fin:
            for line in fin.readlines():
                if "=" in line:
                    tokens = line.split("=")
                    config[tokens[0].strip()] = tokens[1].strip()
        return float(config["Score"])

    submit_num = len(glob.glob(result_dir + "/*"))
    result = [get_score_testcase(i) for i in range(1, 1 + num_testcase)]
    with open(score_template.format(result_dir, submit_num), 'w') as fout:
        for score in result:
            fout.write("{}\n".format(score))


def get_raw_score_list():
    def get_score(num_submit):
        with open(score_template.format(result_dir, num_submit)) as fin:
            return list(map(lambda x: float(x.strip()), fin.readlines()))
    return [get_score(i) for i in range(len(glob.glob(result_dir + "/*")))]


def get_score_list(raw_score_list):

    def score(raw_score, max_raw_score):
        return sum(map(lambda x: 0.0 if x[0] == -1.0 else float(x[0]) / x[1], zip(raw_score, max_raw_score))) * 1000000.0 / num_testcase

    max_raw_score = list(map(max, map(list, list(zip(*raw_score_list)))))

    return [score(raw_score, max_raw_score) for raw_score in raw_score_list]


def print_result():

    score_list = get_score_list(get_raw_score_list())
    score_index = [(score, index) for (index, score) in enumerate(score_list)]
    score_index.sort(reverse=True)
    score_rank_index = [(rank, x[0], x[1])
                        for (rank, x) in enumerate(score_index, start=1)]

    print("| rank | # submit | score |")
    print("|--|--|--|")
    for (rank, score, index) in score_rank_index:
        print("| {} | {} | {} |".format(rank, index, score))


if __name__ == "__main__":
    command = sys.argv[1]

    if "push" == command:
        push_result()
        print_result()
    elif "result" == command:
        print_result()
