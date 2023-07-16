@echo off
git pull origin work
echo ------------------------------------------- >> ErrorsLog.txt
echo %date% >> ErrorsLog.txt
echo %time% >> ErrorsLog.txt
python app4test.py >> ErrorsLog.txt