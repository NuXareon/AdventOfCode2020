# Day 4: Passport Processing

A lot of input validation. Can be a bit tedious so I toyed a bit on how could we make it a bit less repetitive and error prone on the code.

Tried implementing a couple of macros to write all the parsing and checking for the fields. They worked incredibly well actually, even though I tend to never like using macros. By using macros we can parse the input, validate it and write the correct result in just one line, without the risk of calling the wrong function or making a type on the variable name (very easy to do when you have so many inputs and you are copy and pasting).

The other approach would be to use a a function template to parse the different type of inputs. This also works but since we lack reflection here we still need to manually call the right validation function and set the variable, which can be error prone.

There is possibly the option of making each field a custom class instead of a bool, then each field will have it's own parsing and validation. I didn't go too deep into that approach since it makes the code more complex and it is really not worth it for this problem, but it's an approach that would probably work similar to the macro one. The downside is that we need to write a custom class for all the fields which ends up being very tedious.