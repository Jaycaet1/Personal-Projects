Description

This project used 3 LDR sensors, a Humidity Sensor, a Temperature sensor, an Infrared sensor, and an ESP32 microcontroller to create a prototype of a Smart Old Age Home. The sensors send their readings to the microcontroller, which processes them and sends them to the Database and the web page.

Usage

Before using the files, first run the following commands on the terminal within the Smart Old Age Home repository
1. pip install virtualenv #to create a virtual environment to run the project.
2. virtualenv env #to log into your virtual environment.
3. env/Scripts/activate.ps1 #to activate your virtual environment.
4. [If you experience an error message when running the previous command]
   4.1. Windows ~ open PowerShell as an administrator and run: Set-ExecutionPolicy -Scope CurrentUser -ExecutionPolicy Unrestricted
   4.2. Mac & Linux ~ pwsh -Command "Set-ExecutionPolicy -Scope Process -ExecutionPolicy Unrestricted"
6. pip install flask #for creating the web application.
7. pip install mysql-connector-python #For connecting to the MySQL database.
8. pip install matplotlib #For generating graphs within our web application to display sensor data. 
