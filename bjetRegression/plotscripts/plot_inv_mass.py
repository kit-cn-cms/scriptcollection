import ROOT
ROOT.gROOT.SetBatch(False)
ROOT.gStyle.SetOptStat(0);

inputfile1 = ROOT.TFile("ttHbb.root")
#inputfile1 = ROOT.TFile("ttbar.root")

tree1 = inputfile1.Get("MVATree")

isttH = True

if isttH:
    histo_noreg = ROOT.TH1F("noreg","bbMass_noreg",125,50,250)
    histo_reg = ROOT.TH1F("reg","bbMass_reg",125,50,250)
else:
    histo_noreg = ROOT.TH1F("noreg","hadtopmass_noreg",150,100,300)
    histo_reg = ROOT.TH1F("reg","hadtopmass_reg",150,100,300)


c1 = ROOT.TCanvas()

selection_Odd = "Evt_Odd == 0"
selection_ttH = "Evt_bbarMass > 0"
selection_ttbar = "Evt_hadtopmass > 0"


selection = "1"
if isttH:
    selection = selection+ " && "+selection_ttH
    variable_noreg = "Evt_bbarMass"
    variable_reg = "Evt_regbbarMass"
else:
    selection = selection+ " && "+selection_Odd+" && "+selection_ttbar
    variable_noreg = "Evt_hadtopmass"
    variable_reg = "Evt_reghadtopmass"


print "Projecting to noreg" 
tree1.Project("noreg",variable_noreg,selection)
print "Projecting to  reg" 
tree1.Project("reg",variable_reg,selection)


histo_noreg.SetLineColor(ROOT.kBlue)
histo_reg.SetLineColor(ROOT.kRed)

dofit = True

if dofit:
    if isttH:
        noreg_left = 90
        noreg_right = 142
        reg_left = 100
        reg_right = 150
    else:
        noreg_left = 145
        noreg_right = 195
        reg_left = 155
        reg_right = 200

    fit_noreg = ROOT.TF1("Fit_noreg","gaus",noreg_left,noreg_right)
    fit_noreg.SetParameter(0,1)
    fit_noreg.SetParameter(1,1)
    fit_noreg.SetParameter(2,1.5)
    fit_noreg.SetLineWidth(2)
    fit_noreg.SetLineColor(ROOT.kBlue)
    fit_reg = ROOT.TF1("Fit_reg","gaus",reg_left,reg_right)
    fit_reg.SetParameter(0,1)
    fit_reg.SetParameter(1,1)
    fit_reg.SetParameter(2,1.5)
    fit_reg.SetLineWidth(2)
    fit_reg.SetLineColor(ROOT.kRed)


    histo_noreg.Fit("Fit_noreg","","",noreg_left,noreg_right)
    histo_reg.Fit("Fit_reg","","",reg_left,reg_right)


leg=ROOT.TLegend(0.6,0.54,0.89,0.89)
leg.AddEntry(histo_noreg,"Vor Regression")
if dofit:
    leg.AddEntry(0,"Fit: #mu="+str(fit_noreg.GetParameter(1))[:6]+"#pm"+str(fit_noreg.GetParError(1))[:6],"")
    leg.AddEntry(0,"     #sigma="+str(fit_noreg.GetParameter(2))[:6]+"#pm"+str(fit_noreg.GetParError(2))[:6],"")
leg.AddEntry(histo_reg,"Nach Regression")
if dofit:
    leg.AddEntry(0,"Fit: #mu="+str(fit_reg.GetParameter(1))[:6]+"#pm"+str(fit_reg.GetParError(1))[:6],"")
    leg.AddEntry(0,"     #sigma="+str(fit_reg.GetParameter(2))[:6]+"#pm"+str(fit_reg.GetParError(2))[:6],"")


#histo_noreg.GetYaxis().SetRangeUser(0,)

cms = ROOT.TLatex(0.1, 0.92, 'CMS private work')
cms.SetTextFont(42)
cms.SetTextSize(0.05)
cms.SetNDC()


histo_reg.Draw("histoe")
#Make Style
histo_reg.SetTitle("")
histo_reg.GetXaxis().SetTitle("m(b#bar{b})")
histo_reg.GetXaxis().SetTitleSize(0.05)
histo_reg.GetXaxis().SetTitleOffset(0.7)
histo_reg.GetYaxis().SetTitle("Ereignisse")
histo_reg.GetYaxis().SetTitleSize(0.05)
histo_reg.GetYaxis().SetTitleOffset(0.8)


histo_noreg.Draw("same histoe")

if dofit:
    fit_noreg.Draw("same")
    fit_reg.Draw("same")
leg.Draw("same")
cms.Draw("same")
c1.Update()

drawLine = False
if drawLine:
    line = ROOT.TLine(1,0,1,c1.GetUymax())
    line.SetLineWidth(1)
    line.SetLineStyle(2)
    line.SetLineColor(ROOT.kBlack)
    line.Draw("same")

c1.Update()

c1.Print("bbbarmass.pdf","title")

raw_input("Press ret")
