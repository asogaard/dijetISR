import os
import ROOT
import argparse
import rootlogon
from ROOT import *
ROOT.gROOT.LoadMacro("AtlasStyle.C")
ROOT.gROOT.LoadMacro("AtlasLabels.C")
SetAtlasStyle()

parser = argparse.ArgumentParser(description="Plot dijetISR stuff")
parser.add_argument('-i', '--input', dest='infile', required=True)
parser.add_argument('-v', '--variable', dest='variable', required=True)
parser.add_argument('-o', '--output', dest='outname', required=True)
parser.add_argument('--nbinsx', dest='nbinsx', type=int, required=True)
parser.add_argument('--xmin', dest='xmin', type=float, required=True)
parser.add_argument('--xmax', dest='xmax', type=float, required=True)
parser.add_argument('--logx', dest='logx', action='store_true')
parser.add_argument('--logy', dest='logy', action='store_true')
parser.add_argument('-s', '--selection', dest='selection', default='')
parser.add_argument('-x', '--xaxis', dest='xaxis', default='')
parser.add_argument('-y', '--yaxis', dest='yaxis', default='')
parser.add_argument('-d', '--drawOptions', dest='drawOptions', default='')
args = parser.parse_args()

f = ROOT.TFile(args.infile)

t = f.Get("TinyTree")

c = ROOT.TCanvas("c", "c", 600, 600)

t.Draw(args.variable + ">>htemp(%i,%f,%f)" % (args.nbinsx, args.xmin, args.xmax), args.selection, args.drawOptions)

h = c.FindObject('htemp')
h.GetXaxis().SetTitle(args.xaxis)
h.GetYaxis().SetTitle(args.yaxis)

if args.logx:
    c.SetLogx(True)

if args.logy:
    c.SetLogy(True)

l = ROOT.TLatex()
l.SetNDC()
l.SetTextFont(42)
l.SetTextSize(0.04)
l.SetTextColor(1)
l.SetTextAlign(12)
l.DrawLatex(0.5, 0.9, "#bf{#it{ATLAS}} Internal")
l.DrawLatex(0.5, 0.85, "#sqrt{s} = 13 TeV, 13ish fb^{-1}")

leg = ROOT.TLegend(0.55, 0.75, 0.75, 0.8)
leg.SetTextSize(0.03)
leg.AddEntry(h, "Data", "pe")
leg.Draw("same")

c.SaveAs("%s.png" % args.outname)
c.SaveAs("%s.eps" % args.outname)
c.SaveAs("%s.pdf" % args.outname)

raw_input("Press enter to close.")

f.Close()
