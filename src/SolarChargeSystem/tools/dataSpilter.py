import math
import os
column = 5
dataIndex = 1
print("OK")
with open('./log-04.csv') as f:
    contents = f.read()
    startIndex = 0
    lineStartIndex = 0
    commaCount = 0

    output = open('./output.csv',"w")
    while True:
        #print("=====")
        targetIndex = contents.find(",",startIndex)
        #print(targetIndex,"ta")
        if targetIndex == -1:
            print("Finish")
            break
        startIndex = targetIndex + 1
        commaCount += 1
        #print(commaCount,"cc")
        if commaCount == column+1:
            #print("--")
            commaCount = 0
            # 取幾位數
            dataIndexLen = int(math.log10(dataIndex)) +1

            #print(dataIndexLen,"TI")
            
            spiltStartIndex = lineStartIndex
            splitEndIndex = int(targetIndex-dataIndexLen)

            #print(spiltStartIndex,"st")
            #print(splitEndIndex,"end")

            rowStr = contents[spiltStartIndex:splitEndIndex]
            print(dataIndex,"DataIndex")
            dataIndex += 1
            print(rowStr)
            output.write(rowStr)
            output.write("\n")
            # 供下一次使用
            lineStartIndex =  splitEndIndex
            startIndex = splitEndIndex+dataIndexLen
            
            #os.system("pause")
    print("OK")
    output.close()