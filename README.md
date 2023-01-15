# Sensor(Keil studio)
・NUCLEO F446REを使用してBMP390(Adafruit)のライブラリを作りました。(I2C通信用)
・yuki kimuraさんのBMP280(https://os.mbed.com/users/kim1212/code/BMP280/)を参考にしました。
・getpress()で気圧，gettemp()で気温を取得できます。
・ピン名，係数などは適宜，変えてください。
・流れとして
  1. コンストラクタでi2c通信の設定，補償値係数を読み込む
  2. init関数で各係数の設定，モードの設定をする
  3. update関数で繰り返し読み込む
  です。

・BMP390 (Adafruit) library using NUCLEO F446RE. (for I2C communication)
・I used Yuki Kimura's BMP280(https://os.mbed.com/users/kim1212/code/BMP280/) as a reference.
・You can get the air pressure with getpress() and the air temperature with gettemp().
・Please change pin names, coefficients, etc. as necessary.
・The flow of the code is as follows
  1. read i2c communication settings and compensation value coefficients in the constructor
　2. Set each coefficient and mode in the init function. 
　3. Repeatedly read in the update function 
 
