# idrive-controller

This will eventually contain a library to allow simple use of an idrive controller unit form a late model (post2004) bmw. 

The unit is connected via canbus at 500k not 100 as the earlier units were.

my unit had a 4 pin plug on the base with connections 

1 - +12v

2 - GND

3 - can H

4 - can L

i used a arduino with a mcp2515 , along with the mcnight canbus library.
a non library demo sketch is also here to make porting to other can controllers easier.
