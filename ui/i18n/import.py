import os, sys, shutil

print(str(sys.argv))
sourceDir = sys.argv[1]
destDir = sys.argv[2]

names = {
	"be" : "be_BY",
	"cs" : "cs_CZ",
	"de" : "de_DE",
	"es-ES" : "es_ES",
	"fi" : "fi_FI",
	"fr" : "fr_FR",
	"id" : "id_ID",
	"it" : "it_IT",
	"ja" : "ja_JP",
	"ko" : "ko_KR",
	"nl" : "nl_NL",
	"ru" : "ru_RU",
	"sr" : "sr_SR",
	"sv-SE": "sv_SE",
	"th" : "th_TH",
	"tr" : "tr_TR",
	"vi" : "vi_VI",
	"zh-CN" : "zh_CN"
}



dirs = os.listdir(sourceDir)

for d in dirs:
	if d in names:
		spath = sourceDir + "/" + d + "/en_US.ts"
		print(spath)
		dpath = destDir + "/" + names[d] + ".ts"
		print(dpath)
		shutil.copyfile(spath, dpath)


