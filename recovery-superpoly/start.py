import os
import sys
path_split =  f"tmp/sage/after_split"
path_add =  f"tmp/sage/after_add"
path_0 =   f"tmp/may_valuable_superpolys"
path_1 =  f"tmp/sage/after_reduce_first"
path_2 =  f"tmp/sage/after_reduce_second"
path_3 =  f"tmp/sage/after_reduce_third"
path_out =  f"tmp/sage/superpolyinfo"


#call C++ to recover superpoly
def recover_superpoly():
	read = open("subcubes.txt",'r')
	line = read.readline()
	while line:
		numbers = line.split()
		if (len(numbers) > 3):
			num3 = numbers[3]
		else:
			num3 = -1


		if (len(numbers) > 2):
			num2 = numbers[2]
		else:
			num2 = -1

		if (len(numbers) > 1):
			num1 = numbers[1]
		else:
			num1 = -1

		if (len(numbers) > 0):
			num0 = numbers[0]
		else:
			num0 = -1

		os.system('rm nohup.out')
		os.system('rm tmp/TERM/*')
		os.system('rm tmp/STATE/*')
		os.system('rm tmp/LOG/*')
		os.system(f'./trivium {num3} {num2} {num1} {num0}')
		os.system(f'./evaluateRes {num3} {num2} {num1} {num0} ./tmp/TERM')
		line = read.readline()
	read.close()

def split_first():
    for txt in os.listdir(path_0):
        os.system(f'mkdir {path_split}/{txt[:-4]}')
        read = open(f"{path_0}/{txt}",'r')
        allines = read.readlines()

        file_num = 0
        file_size = 0

        write = open(f"{path_split}/{txt[:-4]}/{file_num}.txt", 'w')
        for i in allines:
            if file_size>0 and file_size+len(i)>2000:
                write.close()
                file_num = file_num + 1
                write = open(f"{path_split}/{txt[:-4]}/{file_num}.txt", 'w')
                write.write(i)
                file_size = len(i)
            else:
                file_size = file_size + len(i)
                write.write(i)

        write.close()
        read.close()
def reduce_first():
    for dir in os.listdir(path_split):
        os.system(f'mkdir {path_1}/{dir}')
        path_tmp =  path_split + "/" + dir
        for txt in os.listdir(path_tmp):
            read = open(f"{path_tmp}/{txt}",'r')
            lines = read.readlines()
            write = open("tmp.sage",'w')
            poly = ''
            for line in lines:
                for string in line:
                    if string != "\n":
                        poly = poly + string
                poly = poly + '+'

            poly = poly + '0'

            poly2 = ''
            k_mark = 0
            for j in poly:
                if j == '(':
                    k_mark = 0
                if j == '+':
                    k_mark = 0
                if j == 'k':
                    k_mark = k_mark + 1
                if k_mark > 1 and j=='k':
                    poly2 = poly2 + '*'
                poly2 = poly2 + j

            write.write("PR.<k0,k1,k2,k3,k4,k5,k6,k7,k8,k9,k10,k11,k12,k13,k14,k15,k16,k17,k18,k19,k20,k21,k22,k23,k24,k25,k26,k27,k28,k29,k30,k31,k32,k33,k34,k35,k36,k37,k38,k39,k40,k41,k42,k43,k44,k45,k46,k47,k48,k49,k50,k51,k52,k53,k54,k55,k56,k57,k58,k59,k60,k61,k62,k63,k64,k65,k66,k67,k68,k69,k70,k71,k72,k73,k74,k75,k76,k77,k78,k79,k80> = PolynomialRing(ZZ)\n")
            write.write("P = ")
            write.write(poly2)
            write.write("\n")
            write.write("print(P)")
            write.close()
            read.close()
            os.system(f'sage tmp.sage > {path_1}/{dir}/{txt}')
def reduce_second():
    for dir in os.listdir(path_1):
        path_tmp =  path_1 + "/" + dir
        os.system(f'mkdir {path_2}/{dir}')
        for txt in os.listdir(path_tmp):
            read = open(f"{path_tmp}/{txt}",'r')
            write = open(f"tmp.sage", 'w')

            poly = ''
            line = read.readline()
            pow_mark = 0

            allequs = line.split(" + ")
            for equ in allequs:
                nozero = 0
                if (len(equ) >= 2):
                    if equ[0] == 'k':
                        nozero = 1
                        pow_mark = 0
                        for onechar in equ:
                            if onechar == '^':
                                pow_mark = 1
                            if onechar == '*':
                                pow_mark = 0
                            if pow_mark != 1:
                                poly = poly + onechar


                    else:
                        if '*' in equ:
                            term = equ.split("*")
                            if int(term[0]) % 2 == 1:
                                nozero = 1
                                begin = 0
                                if 'k' in term[1]:
                                    for one_term in term[1:]:
                                        if begin == 1:
                                            poly = poly + '*'
                                        begin = 1
                                        pow_mark = 0
                                        for onechar in one_term:
                                            if onechar == '^':
                                                pow_mark = 1
                                            if pow_mark != 1:
                                                poly = poly + onechar

                        else:
                            nozero = 1
                            poly = poly + equ[:-1]


                if nozero == 1:
                    poly = poly + "+"
            poly = poly + "0"
            write.write(
                "PR.<k0,k1,k2,k3,k4,k5,k6,k7,k8,k9,k10,k11,k12,k13,k14,k15,k16,k17,k18,k19,k20,k21,k22,k23,k24,k25,k26,k27,k28,k29,k30,k31,k32,k33,k34,k35,k36,k37,k38,k39,k40,k41,k42,k43,k44,k45,k46,k47,k48,k49,k50,k51,k52,k53,k54,k55,k56,k57,k58,k59,k60,k61,k62,k63,k64,k65,k66,k67,k68,k69,k70,k71,k72,k73,k74,k75,k76,k77,k78,k79,k80> = PolynomialRing(ZZ)\n")
            write.write("P = ")
            write.write(poly)
            write.write("\n")
            write.write("print(P)")
            write.close()
            read.close()
            os.system(f'sage tmp.sage > {path_2}/{dir}/{txt}')
def reduce_third():
    for dir in os.listdir(path_2):
        os.system(f'mkdir {path_3}/{dir}')
        path_tmp =  path_2 + "/" + dir
        for txt in os.listdir(path_tmp):
            read = open(f"{path_tmp}/{txt}",'r')
            poly = ''
            write = open(f"{path_3}/{dir}/{txt}", 'w')
            line = read.readline()
            pow_mark = 0
            allequs = line.split(" + ")
            for i in allequs:
                if len(i)>0:
                    if i[0] != 'k':
                        if '*' in i:
                            term = i.split("*")
                            if int(term[0]) % 2 == 0:
                                continue
                            else:
                                num_leng = len(term[0])+1
                                write.write(i[num_leng:])
                                write.write("\n")
                                continue
                        else:
                            if int(i) % 2 == 0:
                                continue
                            else:
                                write.write(i)
                                write.write("\n")
                    else:
                        write.write(i)
                        write.write("\n")
            read.close()
            write.close()
def add_all():
    for dir in os.listdir(path_3):
        write = open(f"{path_add}/{dir}.txt", 'w')
        dictall = {}
        path_tmp = path_3 + "/" + dir
        const_num = 0
        wrong = 0
        for txt in os.listdir(path_tmp):
            read = open(f"{path_tmp}/{txt}", 'r')

            lines = read.readlines()
            if len(lines) <1:
                wrong = 1
                break
            for aline in lines:
                if len(aline)>1:
                    if 'k' not in aline:
                        const_num = const_num + int(aline[:-1])
                    else:
                        dictall.setdefault(aline[:-1])
                        if dictall[aline[:-1]] == None:
                            dictall[aline[:-1]] = 1
                        else:
                            num = dictall[aline[:-1]]
                            num = num + 1
                            dictall[aline[:-1]] = num
        if (wrong == 1):
            write = open(f"{path_add}.txt", 'w')
            write.close()
            continue
        for j in dictall:
            if dictall[j]%2 != 0:
                write.write(j)
                write.write("\n")
        write.write(str(const_num))
        write.close()
def write_core():
    write = open(f"{path_out}.txt", 'w')
    write.close()
    for txt in os.listdir(path_add):
        read = open(f"{path_add}/{txt}",'r')
        write = open(f"{path_out}.txt", 'a')
        allline = read.readlines()

        term = []

        for i in range(0,81):
            term.append('k')

        term_num = [0] * 81

        for i in range(0,81):
            term[i] = term[i] + str(i)

            for aline in allline:
                aline_terms = aline[:-1].split("*")
                if term[i]  in aline_terms:
                    index = int(term[i][1:])
                    term_num[index] = term_num[index] + 1
        write.write(f"{txt}\n")
        write.write("balance:")
        for i in range(0,81):
            if term_num[i] == 1 :
                for aline in allline:
                    all_term = aline[:-1].split("*")
                    pnum = len(all_term)
                    if pnum == 1 and term[i] in all_term:
                        write.write(f"{i};")

        write.write("\n")
        write.write("quadratic:")
        for i in range(0, 81):
            have_up3 = 0
            have_2 = 0
            if term_num[i] == 2 or term_num[i] == 1:

                double_term = []
                for aline in allline:
                    all_term = aline[:-1].split("*")
                    pnum = len(all_term)
                    if term[i] in all_term:
                        double_term.append(aline[:-1])
                        if pnum >= 3:
                            have_up3 = 1
                        if pnum == 2:
                            have_2 = 1
                if have_up3 != 1 and have_2 == 1:
                    write.write(f"k{i}:")
                    write.write(double_term[0])
                    if term_num[i] == 2:
                        write.write("+")
                        write.write(double_term[1])
                    write.write(";\t")

        write.write("\n")
        write.write("all:")
        allnum = 0
        for i in range(0,81):

            if term_num[i] >= 1:
                allnum = allnum + 1
                write.write(f"{i};")
        write.write("\n")
        write.write(f"allnum:{allnum}\n")
        write.write("\n")
        write.close()
        read.close()



def cancel1():
    write = open("tmp/quadratic/quadratic_out.txt",'w')
    read = open("result/quadratic_info.txt",'r')
    lines = read.readlines()
    for aline in lines:
        if "quadratic" in aline:
            write.write(aline[10:])
    write.close()
    read.close()
def cancel2():
    write = open("tmp/quadratic/quadratic_out_2.txt",'w')
    read = open("tmp/quadratic/quadratic_out.txt",'r')
    lines = read.readlines()
    for aline in lines:
        c = aline[:-1].split(";")
        for jj in c:
            if 'k'in jj:
                for jjj in jj:
                    if jjj != "\t":
                        write.write(str(jjj))
                write.write("\n")
    write.close()
    read.close()
def cancel3():
    write = open("tmp/quadratic/quadratic_out_3.txt",'w')
    read = open("tmp/quadratic/quadratic_out_2.txt",'r')
    dictall = {}

    lines = read.readlines()
    for aline in lines:
        if len(aline)>= 3:
            c = aline[:-1].split(":")
            nums = []
            nums.append(c[1])
            dictall.setdefault(c[0])
            if dictall[c[0]] == None:
                dictall[c[0]] = nums
            else:
                tmp = [x for x in dictall[c[0]]]
                tmp.append(c[1])
                dictall[c[0]] = tmp

    all_pair = []
    for jj in dictall:

        for equ in dictall[jj]:
            for equ2 in dictall[jj]:
                if equ in equ2 and len(equ) < len(equ2) and (len(equ) +5) > len(equ2) and "+" in equ2:
                    pair = []
                    pair.append(equ)
                    pair.append(equ2)
                    if pair not in all_pair:
                        all_pair.append(pair)

    for jj in all_pair:
        write.write(jj[0]+" ")
        write.write(jj[1]+"\n")



    write.close()
    read.close()
def cancel4():
    write = open("result/balance_from_quadratic.txt",'w')

    read2 = open("tmp/quadratic/quadratic_out_3.txt",'r')
    lines22 = read2.readlines()
    for alines22 in lines22:
        alines22_2 = alines22[:-1].split(" ")

        dictall = {}
        dictall2 ={}

        read = open("result/quadratic_info.txt",'r')
        char = alines22_2[0]
        char2 = alines22_2[1]


        dictall.setdefault(char)
        dictall2.setdefault(char2)
        lines = read.readlines()


        for i in range(0,len(lines)):
            if "quadratic" in lines[i] and len(lines[i])> 12:
                a = lines[i][10:].split(";")
                for a1 in a[:-1]:
                    cc = a1.split(":")
                    b = a1.split(":")[1]
                    if char == b:
                        name = (lines[i-1][:-1])
                        num = []
                        nums = lines[i+1][:-3].split(" ")
                        for anum in nums:
                            num.append(int(anum))

                        alls = []
                        all = []
                        all.append(name)
                        all.append(nums)
                        alls.append(all)

                        if dictall[char] == None:
                            dictall[char] = alls
                        else:
                            tmp = [x for x in dictall[char]]
                            chongfu = 0

                            for tmp2 in tmp:
                                if all[1] == tmp2[1]:
                                    chongfu = 1
                                    break
                            if chongfu == 0:
                                tmp.append(all)
                                dictall[char] = tmp
                        break


        for i in range(0,len(lines)):
            if "quadratic" in lines[i] and len(lines[i])> 12:
                a = lines[i][10:].split(";")
                for a1 in a[:-1]:
                    b = a1.split(":")[1]
                    if char2 == b:
                        name = (lines[i-1][:-1])
                        num = []
                        nums = lines[i+1][:-3].split(" ")
                        for anum in nums:
                            num.append(int(anum))

                        alls = []
                        all = []
                        all.append(name)
                        all.append(nums)
                        alls.append(all)

                        if dictall2[char2] == None:
                            dictall2[char2] = alls
                        else:
                            tmp = [x for x in dictall2[char2]]
                            chongfu = 0

                            for tmp2 in tmp:
                                if all[1] == tmp2[1]:
                                    chongfu = 1
                                    break
                            if chongfu == 0:
                                tmp.append(all)
                                dictall2[char2] = tmp
                        break


        linear_num = char2.split("+")[1]
        for ii in dictall[char]:
            for jj in dictall2[char2]:
                if 'k' not in linear_num[1:]:
                    write.write(str(ii[0])+"+")
                    write.write(str(jj[0])+"\n")
                    all_num_ = []
                    for zz in ii[1]:
                        if zz not in all_num_:
                            all_num_.append(zz)
                    for zz in jj[1]:
                        if zz not in all_num_:
                            all_num_.append(zz)
                    all_num_.sort()
                    write.write("balance:")
                    write.write(linear_num[1:])
                    write.write("  \n")
                    for zz in all_num_:
                        write.write(str(zz)+" ")
                    write.write(" \n")
                    write.write("allnum:"+str(len(all_num_))+"\n"+"\n")
        read.close()

    write.close()
    read2.close()


#use sage to extract information from superpoly
def sage_extract_information():
    split_first()
    reduce_first()
    reduce_second()
    reduce_third()
    add_all()
    write_core()


def extract_balance_and_quadratic():
    max = 80
    min = 0
    read = open("tmp/sage/superpolyinfo.txt", 'r')
    w_quadratic = open("result/quadratic_info.txt", 'a')
    w_balance = open("result/balance_info.txt", 'a')

    lines = read.readlines()
    i = 0

    while i < len(lines):
        num_all = int((lines[i + 4].split(":"))[1][:-1])
        if num_all > min and num_all <= max:
            if len(lines[i + 1]) > 9:
                num = lines[i + 1][8:-1].split(";")
                w_balance.write(lines[i])
                w_balance.write("balanced:")
                for j in num:
                    w_balance.write(j)
                    w_balance.write(" ")
                w_balance.write("\n")
                num = lines[i + 3][4:-1].split(";")
                for j in num:
                    w_balance.write(j)
                    w_balance.write(" ")
                w_balance.write("\n")
                w_balance.write(lines[i + 4])
                w_balance.write("\n")

            if len(lines[i + 2]) > 11:
                w_quadratic.write(lines[i])
                w_quadratic.write("quadratic:")
                w_quadratic.write(lines[i + 2][10:])

                num = lines[i + 3][4:-1].split(";")
                for j in num:
                    w_quadratic.write(j)
                    w_quadratic.write(" ")
                w_quadratic.write("\n")
                w_quadratic.write(lines[i + 4])
                w_quadratic.write("\n")
        i = i + 6
    w_quadratic.close()
    w_balance.close()
    read.close()

def canceling_quadratic_term():
    cancel1()
    cancel2()
    cancel3()
    cancel4()


if __name__ == '__main__':
    os.system(f'sh exec.sh')
    recover_superpoly()
    sage_extract_information()
    extract_balance_and_quadratic()
    canceling_quadratic_term()
    os.system(f'cp -R -f tmp/may_valuable_superpolys/* result/superpoly/may_valuable_superpolys/')