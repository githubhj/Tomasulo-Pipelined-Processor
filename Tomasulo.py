#!/usr/bin/env python
import os
import subprocess
import re
import shlex

if __name__ == "__main__":
    
    '''
    For gcc.100k.trace
    '''
    
    procsim = "./procsim"
    fetch_rate =4
    k0_num = 3
    k1_num = 2
    k2_num =1
    result_bus = 2

    
    trace1 = "gcc.100k.trace"
    temp1 = re.split("\.",trace1)
    print "Running simulations for",trace1,"......" 
    count =1
    result_cycles = []
    log_path = "/home/harshit/git/procsim/log_gcc"
    os.mkdir(log_path)
    for f in range(8,3,-4):
        for k0 in range(2,0,-1):
            for k1 in range(2,0,-1):
                for k2 in range(2,0,-1):
                    for r in range(9,0,-1):
                        runcommand = procsim + " -f " + str(f) + " -j " + str(k0) + " -k " + str(k1) + " -l " + str(k2) + " -r " + str(r)
                        runcommand = runcommand + " -i ./traces/" + trace1
                        log = log_path + "/simulation_f" + str(f) + "_k0" + str(k0) + "_k1" + str(k1) + "_k2" + str(k2) + "_r" + str(r) + "_" + temp1[0] + ".output"
                        print "SIMULATION",count,": ",runcommand
                        count+=1
                        args = shlex.split(runcommand)
                        myoutput= open(log,'w')
                        process1 = subprocess.Popen(args, stdout = myoutput, cwd =r'/home/harshit/git/procsim')
                        process1.wait()
                        myoutput.flush();
                        myoutput.close();
                        print "***********Done***********\n\n\n"
                        readlog = open(log,'r')
                        lines = readlog.readlines()
                        readlog.close()
                        temp_result = lines[-1].strip()
                        temp_result_array = re.split(" ", temp_result)
                        if(len(result_cycles)==0):
                            result_cycles.append(float(temp_result_array[-1]))
                    
                        elif ((float(temp_result_array[-1])*(0.95)) <= float(min(result_cycles))):
                            result_cycles.append(float(temp_result_array[-1]))
                            
                        else:
                            os.remove(log)
                                                                                 
    trace2 = "gobmk.100k.trace"
    temp1 = re.split("\.",trace2)
    print "Running simulations for",trace2,"......" 
    count =1
    result_cycles = []
    log_path = "/home/harshit/git/procsim/log_gobmk"
    os.mkdir(log_path)
    for f in range(8,3,-4):
        for k0 in range(2,0,-1):
            for k1 in range(2,0,-1):
                for k2 in range(2,0,-1):
                    for r in range(9,0,-1):
                        runcommand = procsim + " -f " + str(f) + " -j " + str(k0) + " -k " + str(k1) + " -l " + str(k2) + " -r " + str(r)
                        runcommand = runcommand + " -i ./traces/" + trace2
                        log = log_path + "/simulation_f" + str(f) + "_k0" + str(k0) + "_k1" + str(k1) + "_k2" + str(k2) + "_r" + str(r) + "_" + temp1[0] + ".output"
                        print "SIMULATION",count,": ",runcommand
                        count+=1
                        args = shlex.split(runcommand)
                        myoutput= open(log,'w')
                        process1 = subprocess.Popen(args, stdout = myoutput, cwd =r'/home/harshit/git/procsim')
                        process1.wait()
                        myoutput.flush();
                        myoutput.close();
                        print "***********Done***********\n\n\n"
                        readlog = open(log,'r')
                        lines = readlog.readlines()
                        readlog.close()
                        temp_result = lines[-1].strip()
                        temp_result_array = re.split(" ", temp_result)
                        if(len(result_cycles)==0):
                            result_cycles.append(float(temp_result_array[-1]))
                    
                        elif ((float(temp_result_array[-1])*0.95) <= float(min(result_cycles))):
                            result_cycles.append(float(temp_result_array[-1]))
                            
                        else:
                            os.remove(log)
    
    trace3 = "hmmer.100k.trace"
    temp1 = re.split("\.",trace3)
    print "Running simulations for",trace3,"......" 
    count =1
    result_cycles = []
    log_path = "/home/harshit/git/procsim/log_hmmer"
    os.mkdir(log_path)
    for f in range(8,3,-4):
        for k0 in range(2,0,-1):
            for k1 in range(2,0,-1):
                for k2 in range(2,0,-1):
                    for r in range(9,0,-1):
                        runcommand = procsim + " -f " + str(f) + " -j " + str(k0) + " -k " + str(k1) + " -l " + str(k2) + " -r " + str(r)
                        runcommand = runcommand + " -i ./traces/" + trace3
                        log = log_path + "/simulation_f" + str(f) + "_k0" + str(k0) + "_k1" + str(k1) + "_k2" + str(k2) + "_r" + str(r) + "_" + temp1[0] + ".output"
                        print "SIMULATION",count,": ",runcommand
                        count+=1
                        args = shlex.split(runcommand)
                        myoutput= open(log,'w')
                        process1 = subprocess.Popen(args, stdout = myoutput, cwd =r'/home/harshit/git/procsim')
                        process1.wait()
                        myoutput.flush();
                        myoutput.close();
                        print "***********Done***********\n\n\n"
                        readlog = open(log,'r')
                        lines = readlog.readlines()
                        readlog.close()
                        temp_result = lines[-1].strip()
                        temp_result_array = re.split(" ", temp_result)
                        if(len(result_cycles)==0):
                            result_cycles.append(float(temp_result_array[-1]))
                    
                        elif ((float(temp_result_array[-1])*(0.95)) <= float(min(result_cycles))):
                            result_cycles.append(float(temp_result_array[-1]))
                            
                        else:
                            os.remove(log)
                        
    trace4 = "mcf.100k.trace"
    temp1 = re.split("\.",trace4)
    print "Running simulations for",trace4,"......" 
    count =1
    result_cycles = []
    log_path = "/home/harshit/git/procsim/log_mcf"
    os.mkdir(log_path)
    for f in range(8,3,-4):
        for k0 in range(2,0,-1):
            for k1 in range(2,0,-1):
                for k2 in range(2,0,-1):
                    for r in range(9,0,-1):
                        runcommand = procsim + " -f " + str(f) + " -j " + str(k0) + " -k " + str(k1) + " -l " + str(k2) + " -r " + str(r)
                        runcommand = runcommand + " -i ./traces/" + trace4
                        log = log_path + "/simulation_f" + str(f) + "_k0" + str(k0) + "_k1" + str(k1) + "_k2" + str(k2) + "_r" + str(r) + "_" + temp1[0] + ".output"
                        print "SIMULATION",count,": ",runcommand
                        count+=1
                        args = shlex.split(runcommand)
                        myoutput= open(log,'w')
                        process1 = subprocess.Popen(args, stdout = myoutput, cwd =r'/home/harshit/git/procsim')
                        process1.wait()
                        myoutput.flush();
                        myoutput.close();
                        print "***********Done***********\n\n\n"
                        readlog = open(log,'r')
                        lines = readlog.readlines()
                        readlog.close()
                        temp_result = lines[-1].strip()
                        temp_result_array = re.split(" ", temp_result)
                        if(len(result_cycles)==0):
                            result_cycles.append(float(temp_result_array[-1]))
                    
                        elif ((float(temp_result_array[-1])*(0.95)) <= float(min(result_cycles))):
                            result_cycles.append(float(temp_result_array[-1]))
                            
                        else:
                            os.remove(log)
    
    
