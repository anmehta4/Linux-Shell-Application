I coded it to make sure all the commands are split and put into an array.
Based on the commands I manipulate the array to make sure its compatible for execv.
During this process I also check if the command is compatible.
Then I create a child process and run the respective command in this child process.
A similiar approach is also used for the loop variant of this code.

