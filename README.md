I broke the code.

Project Goal:
Demonstrate my Python scripting, C++ embedded programming, and SystemVerilog proficiency and further build upon this into a more complex project (thinking digital filter)
Have 4 buttons serve as an input into an ALU, the buttons are programmed as multiuse, and since its binary order doesnt matter. Pressing a button once changes it from 0 to 1, and this changes with each press.
This is essentially a state machine
Once the desired input is entered through the buttons, the fifth button is used to read the number
(Storing it as a string, outputting to serial monitor)
Repeated for input 2
Once entered, the five-button switch is used to begin serving as operation select (ADD, SUB, OR, AND, XOR)
This input is converted into a 3-bit output, which ranges from 001 - 101 in the serial monitor, corresponding to the SystemVerilog operation cases
The Python script initially is set to wait until 3 things are in serial monitor, then it copies those into a file which is read by the SystemVerlog program
(To retrieve the output from the ALU, I need to get a different ESP which can read the serial monitor)
There are then 3 options: clearing the ALU and starting fresh, saving the output as input 1, or saving it as input 2
If cleared, the ALU returns to its first state,
If it saves output as input 1, the serial monitor receives a "1," which tells it to only look for 2 more serial lines before printing to the file and to hold the ModelSim Output in the file as input 1
Then the state machine goes back to "phase 1," receiving input2 and the operation, before repeating whats above
If it saves output as input 2,the  serial monitor receives a "0," which tells the Python script that it will ignore the second line of the serial monitor (which will be 0000)
The state machine then moves to state 4, where it takes input 1, skips input2, and asks for the operation before repeating based on the user input after the output
