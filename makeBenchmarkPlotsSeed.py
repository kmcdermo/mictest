import os.path, glob, sys
import ROOT
import array
import math

hORm = sys.argv[1] # snb or knc

g = ROOT.TFile('benchmark_seeding_'+hORm+'.root',"recreate")

# triplet = just triplet finding
# merge = just turning triplets into tracks
# seeding = triplet + merge
# fit = CF and KF fit over seeds in MPlex
# full = seeding + fit

# print "Vectorization"
# for test in ['triplets','merge','seeding','fit','full']:
#     for tracks in ['1000','5000','10000']:
#         print test,tracks
        
#         # Vectorization data points
#         vuvals = ['1','2','4','8']
#         if   hORm == 'snb' :
#             vuvals.append('8int')
#         elif hORm == 'knc' :
#             vuvals.append('16')
#             vuvals.append('16int')
#         else : exit
#         nth = '1'
        
#         #Vectorization time    
#         g_VU = ROOT.TGraphErrors(len(vuvals))
#         g_VU_speedup = ROOT.TGraphErrors(len(vuvals))
#         point = 0
#         for vu in vuvals:
#             if   vu == '8int'  : xval = 8.0
#             elif vu == '16int' : xval = 8.0
#             else               : xval = float(vu)
#             yvals = array.array('d');
#             firstFound = False

#             # Read in times from log files, store into yvals
#             with open('seedingtimes_'+hORm+'_nTH'+nth+'_nVU'+vu+'_nTk'+tracks+'.txt') as f:
#                 for line in f:
#                     lsplit = line.split(',')
#                     if not firstFound:
#                         firstFound = True
#                         continue
#                     yval = 0.0
#                     if   test == 'triplets': yval = float(lsplit[0])
#                     elif test == 'merge'   : yval = float(lsplit[1])
#                     elif test == 'seeding' : yval = (float(lsplit[0])+float(lsplit[1]))
#                     elif test == 'fit'     : yval = float(lsplit[2])
#                     elif test == 'full'    : yval = (float(lsplit[0])+float(lsplit[1])+float(lsplit[2]))
#                     yvals.append(yval)

#             # Compute mean and uncertainty on mean
#             sum = 0.0;
#             for yval in range(0,len(yvals)):
#                 sum = sum + yvals[yval]
#             mean = sum/len(yvals)
#             emean = 0.0;
#             for yval in range(0,len(yvals)):
#                 emean = emean + ((yvals[yval] - mean) * (yvals[yval] - mean))
#             emean = math.sqrt(emean / (len(yvals) - 1))
#             emean = emean/math.sqrt(len(yvals))

#             print xval,mean,'+/-',emean
#             g_VU.SetPoint(point,xval,mean)
#             g_VU.SetPointError(point,0,emean)
#             point = point+1
#         g_VU.Write("g_"+test+"_nTk"+tracks+"_VU")

#         # Vectorization speedup
#         x0 = array.array('d',[0])
#         y0 = array.array('d',[0])
#         g_VU.GetPoint(0,x0,y0)
#         point = 0
#         for vu in vuvals:
#             xval = array.array('d',[0])
#             yval = array.array('d',[0])
#             g_VU.GetPoint(point,xval,yval)
#             speedup = 0.0
#             if yval[0]>0.0: speedup = y0[0]/yval[0]
#             g_VU_speedup.SetPoint(point,xval[0],speedup)
#             point = point+1
#         g_VU_speedup.Write("g_"+test+"_nTk"+tracks+"_VU_speedup")

# do it over again for parallelization

print "Parallelization"
for test in ['triplets','merge','seeding','fit','full']:
    for tracks in ['1000','5000','10000']:
        print test,tracks

        # Parallelization data points
        if   hORm == 'snb' : 
            thvals = ['1','2','4','6','8','12','16','20','24']
            nvu = '8int'
        elif hORm == 'knc' : 
            thvals = ['1','2','4','8','15','30','60','90','120','150','180','210','240']
            nvu = '16int'
        
        #Parallelization time    
        g_TH = ROOT.TGraphErrors(len(thvals))
        g_TH_speedup = ROOT.TGraphErrors(len(thvals))
        point = 0
        for th in thvals:
            xval = float(th)
            yvals = array.array('d');
            firstFound = False

            # Read in times from log files, store into yvals
            with open('seedingtimes_'+hORm+'_nTH'+th+'_nVU'+nvu+'_nTk'+tracks+'.txt') as f:
                for line in f:
                    lsplit = line.split(',')
                    if not firstFound:
                        firstFound = True
                        continue
                    yval = 0.0
                    if   test == 'triplets': yval = float(lsplit[0])
                    elif test == 'merge'   : yval = float(lsplit[1])
                    elif test == 'seeding' : yval = (float(lsplit[0])+float(lsplit[1]))
                    elif test == 'fit'     : yval = float(lsplit[2])
                    elif test == 'full'    : yval = (float(lsplit[0])+float(lsplit[1])+float(lsplit[2]))
                    yvals.append(yval)

            # Compute mean and uncertainty on mean
            sum = 0.0;
            for yval in range(0,len(yvals)):
                sum = sum + yvals[yval]
            mean = sum/len(yvals)
            emean = 0.0;
            for yval in range(0,len(yvals)):
                emean = emean + ((yvals[yval] - mean) * (yvals[yval] - mean))
            emean = math.sqrt(emean / (len(yvals) - 1))
            emean = emean/math.sqrt(len(yvals))

            print xval,mean,'+/-',emean
            g_TH.SetPoint(point,xval,mean)
            g_TH.SetPointError(point,0,emean)
            point = point+1
        g_TH.Write("g_"+test+"_nTk"+tracks+"_TH")

        # Parallelization speedup
        x0 = array.array('d',[0])
        y0 = array.array('d',[0])
        g_TH.GetPoint(0,x0,y0)
        point = 0
        for th in thvals:
            xval = array.array('d',[0])
            yval = array.array('d',[0])
            g_TH.GetPoint(point,xval,yval)
            speedup = 0.0
            if yval[0]>0.0: speedup = y0[0]/yval[0]
            g_TH_speedup.SetPoint(point,xval[0],speedup)
            point = point+1
        g_TH_speedup.Write("g_"+test+"_nTk"+tracks+"_TH_speedup")

g.Write()
g.Close()
