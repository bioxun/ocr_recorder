
MaxCharNum=3000
user_defined_dpi=200

-- ------------------------------------------
--whitelist=""
--blacklist="军官没有"
--blacklist="a "
-- ------------------------------------------
engine_oem=1
-- ------------------------------------------
PageSegMode=4
PageSegMode=6
--PageSegMode=7
--PageSegMode=5
--PageSegMode=12

-- ------------------------------------------
target_loc={451,835,915,32}
--target_loc={490,830,862,38}   -- 棱镜史话
--target_loc={490,828,862,42}
--target_loc={432,761,975,190}
target_loc={498,820,866,51}     -- 江湖弃子
--target_loc={592,792,712,190}
--target_loc={564,552,710,70}

--------------------------------------------
modeScript=0

--------------------------------------------
doDilate=0
r_near=0.4
--------------------------------------------
doLab=1
--------------------------------------------
Rrc_seekColor={0.375,0.25,0.25,0.5}
--Rrc_seekColor={0.3,0.1,0.4,0.8}
--------------------------------------------
doDebug=1
doShow=1

--------------------------------------------
alpha_contrast=1
beta_contrast=10
blockSize1=5    -- 重要
doBlur=1
canny_thresh1=85
canny_thresh2=185
--------------------------------------------
doFlood1=1
doFlood2=doFlood1
colorFloodB={255,255,255}
flood_fillMode=4
flood_connect=4
N_floodPointX=90
N_floodPointY=10
colorFlood_lowDiff=1
colorFlood_upDiff=1
--------------------------------------------
min_area=0
--------------------------------------------
colorTarget={255,255,255,30}
--colorTarget={54,188,252,40}
--colorTarget={1,222,252,50}
colorTarget={0,0,0,60}
--colorTarget={0,1,7,50}

-- ------------------------------------------
auto_colorTarget=1
dist_colorTarget=20
-- ------------------------------------------
preprocessType=3

-- ------------------------------------------
Is_DarkChar=0
--imgFile="Images/pure_white_1.png"
imgFile="Images/example02.png"
imgFile="Images/example03.png"
imgFile="Images/example04.png"
imgFile="Images/example05.png"
imgFile="Images/example11_yellow.png"
imgFile="Images/example19_y.png"
imgFile="Images/example14_white.png"
imgFile="Images/example13.png"
imgFile="Images/example15_quesMark.png"
--imgFile="Images/example18_w.png"
--------------------------Dark-------------------
----Is_DarkChar=1
imgFile="Images/example08.png"
--imgFile="Images/example15_yellow.png"
--imgFile="Images/example10_en.png"
--imgFile="Images/example10_en_Sm.png"
---------------------------
--imgFile="Images/example16_yellow.png"  
--imgFile="Images/example16p_yellow.png"
--imgFile="Images/example17_yellow.png"    --  ?
----------------------------
--imgFile="Images/example01_eng_1line.png"
--imgFile="Images/example06.png"
--imgFile="Images/example06a.png"
------------------------------
--imgFile="Images/example09_idCard.png"
--imgFile="Images/example01.png"
--imgFile="Images/shot_@0313#2340.jpg"


--------------------------------------------
tmp=1
--------------------------------------------
ConfigFile="ocr_tesser.config"
--ConfigFile=""

-- ------------------------------------------
--lang="chi_sim_noDict"    -- language
--lang="chi_sim2"    -- language
--lang="chi_sim_noConfig"    -- language
--lang="chi_all"    -- language
--lang="chi_sim_word"    -- language
--lang="chi_sim_noLstmWord"    -- language
lang="chi_sim+eng"    -- language
--lang="chi_sim"    -- language
--lang="eng+chi_sim"    -- language
--lang="chi_tra"    -- language
--lang="chi_sim+osd"    -- language
--lang="eng"    -- language

--path_tessdata = "G:/softwares_program/0_C/packages/tesseract/tessdata_gumblex"
path_tessdata="D:/ProgKits/msys2/mingw64/share/tessdata"   -- fold with .traindata
path_tessdata = "G:/softwares_program/0_C/packages/tesseract/tessdata_best"
path_tessdata = "G:/softwares_program/0_C/packages/tesseract/tessdata_fast"
path_tessdata = "G:/softwares_program/0_C/packages/tesseract/tessdata_normal"
--path_tessdata = "F:/softwares/Doc_Pic/2_OCR/Capture2Text/Capture2Text/tessdata"  
--path_tessdata = "D:/Program Files/Tesseract-OCR/tessdata"


