import os.path, glob, sys
import ROOT
import array

if len(sys.argv)<2 or len(sys.argv)>3: exit

hORm = sys.argv[1]

isCMSSW = False
if len(sys.argv)>2:
    if sys.argv[2]=="cmssw": isCMSSW=True
    else: exit

if hORm!='host' and hORm!='host_endcap' and hORm!='mic' and hORm!='mic_endcap': exit

g = ROOT.TFile('benchmark_'+hORm+'.root',"recreate")

for test in ['BH','CE','CEST','ST','TBBST','FIT']:
    if isCMSSW and test=='FIT': continue
    if 'endcap' in hORm and not isCMSSW and 'FIT' not in test: continue
    print test
    pos = 14
    ntks = '10x20k'
    nevt = 10.
    if isCMSSW:
        ntks = '100xTTbarPU35'
        nevt = 100.
    if 'BH' in test: pos = 8
    if 'TBB' in test: pos = 17
    if 'ST' == test: pos = 11
    if 'FIT' in test: 
        pos = 3
        ntks = '10x1M'
    g_VU = ROOT.TGraph(4)
    g_VU_speedup = ROOT.TGraph(4)
    point = 0
    vuvals = ['1','2','4','8']
    if 'mic' in hORm: 
        vuvals.append('16')
        vuvals.append('16int')
    else:
        vuvals.append('8int')
    for vu in vuvals:
        os.system('grep Matriplex log_'+hORm+'_'+ntks+'_'+test+'_NVU'+vu+'_NTH1.txt >& log_'+hORm+'_'+ntks+'_'+test+'_VU.txt')
        if vu == '16int':
            xval = 16.0
        elif vu == '8int':
            xval = 8.0
        else:
            xval = float(vu)
        yval = 0.
        firstFound = False
        with open('log_'+hORm+'_'+ntks+'_'+test+'_VU.txt') as f:
            for line in f:
                if 'Matriplex' not in line: continue
                if 'Total' in line: continue
                lsplit = line.split()
                if 'FIT' in test: 
                    tmp = float(lsplit[pos])
                    if firstFound is False or tmp<yval: yval=tmp
                    if not firstFound:
                        firstFound = True
                else:
                    if not firstFound:
                        firstFound = True
                        continue
                    yval = yval+float(lsplit[pos])
        if 'FIT' not in test: yval = nevt*yval/(nevt-1.)
        print xval, yval
        g_VU.SetPoint(point,xval,yval)
        point = point+1
    x0 = array.array('d',[0])
    y0 = array.array('d',[0])
    g_VU.GetPoint(0,x0,y0)
    point = 0
    for vu in vuvals:
        xval = array.array('d',[0])
        yval = array.array('d',[0])
        g_VU.GetPoint(point,xval,yval)
        speedup = 0.
        if yval[0]>0.: speedup = y0[0]/yval[0]
        g_VU_speedup.SetPoint(point,xval[0],speedup)
        point = point+1
    g_VU.Write("g_"+test+"_VU")
    g_VU_speedup.Write("g_"+test+"_VU_speedup")

    point = 0
    nvu = '8int'
    if 'mic' in hORm: nvu = '16int'
    thvals = [1,3,7,21]
    if 'TBB' in test or 'BH' in test : thvals = [1,2,3,4,6,8,10,12,16,20,24]
    if 'mic' in hORm: thvals = [1,3,5,15,30,45,60,75,90,105,120,150,180,210,240]
    g_TH = ROOT.TGraph(len(thvals))
    g_TH_speedup = ROOT.TGraph(len(thvals))
    for th in thvals:
        os.system('grep Matriplex log_'+hORm+'_'+ntks+'_'+test+'_NVU'+nvu+'_NTH'+str(th)+'.txt >& log_'+hORm+'_'+ntks+'_'+test+'_TH.txt')
        xval = float(th)
        yval = 0.
        firstFound = False
        with open('log_'+hORm+'_'+ntks+'_'+test+'_TH.txt') as f:
            for line in f:
                if 'Matriplex' not in line: continue
                if 'Total' in line: continue
                lsplit = line.split()
                if 'FIT' in test: 
                    tmp = float(lsplit[pos])
                    if firstFound is False or tmp<yval: yval=tmp
                    if not firstFound:
                        firstFound = True
                else:
                    if not firstFound:
                        firstFound = True
                        continue
                    yval = yval+float(lsplit[pos])
        if 'FIT' not in test: yval = nevt*yval/(nevt-1.)
        print xval, yval
        g_TH.SetPoint(point,xval,yval)
        point = point+1
    x0 = array.array('d',[0])
    y0 = array.array('d',[0])
    g_TH.GetPoint(0,x0,y0)
    point = 0
    for th in thvals:
        xval = array.array('d',[0])
        yval = array.array('d',[0])
        g_TH.GetPoint(point,xval,yval)
        speedup = 0.
        if yval[0]>0.: speedup = y0[0]/yval[0]
        g_TH_speedup.SetPoint(point,xval[0],speedup)
        point = point+1
    g_TH.Write("g_"+test+"_TH")
    g_TH_speedup.Write("g_"+test+"_TH_speedup")

g.Write()
g.Close()
