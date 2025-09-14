# reflow-oven
Custom reflow oven

![header](https://ckalitin.github.io/assets/images/made-reflow-oven/header.jpg)

### Links

Project Notes:  
https://docs.google.com/document/d/1razJQ7vhXl0n5-KTTxf6exUxA-YA-EfNmRcggGIF1KQ/edit?usp=sharing  

Blog Post:  
https://ckalitin.github.io/ideas/2025/09/14/made-reflow-oven.html  

Posts:  
https://x.com/CKalitin/status/1967310762533347823  
https://x.com/CKalitin/status/1966366769989468180  

### Standard Operating Procedure

Reflow Oven SOP:  
1. Ensure the proper reflow profile is flashed onto the STM32 and that the thermistor is installed in the oven.  
2. Plug in the reflow oven and plug a USB into the STM32. Ensure the UART read script COM port matches the STM32 COM port.  
3. Hold down the reset button on the STM32 and start the UART read script. (This way it’ll start at t=0)  
4. Watch over the reflow oven as it operates.  
5. When the target temperature goes to 0 C, open the door half way. As the slope of the temperature graph beings to decrease, open the door all the way.  
6. Take your PCB out when the temperature has reached a safe level.  

Note that at the low-end, the temperature reading is off by ~10 C, so it reads ~30-40 C normally.
