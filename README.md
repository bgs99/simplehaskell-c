# simplehaskell-c
Simple Haskell-like language interpreter with call-by-name evaluation and basic type system

Current restrictions:
    - Expressions with unnecessary parenthesises won't work
    - Datatypes cannot contain functions (not tested)
    - Datatypes cannot have arguments (no generic datatypes)
    - Functions are processed while reading so visibility is restricted to the rest of the file
        - That means no corecursion
    - Functions may evaluate strangely when argument names clash with globally defined functions
    - Recursion kills interpreter because it is  singlethreaded
    - There's no check for type existance, but functions with such types cannot be evaluated anyway
        - You could actually consider it a feature, in style of Curry–Howard isomorphism
            - I wonder if one can build some logic system on top of that...
    - There's no termination check, either compiletime or runtime
    - No type derivation, but type chcking algorithm is pretty close to it
    - You have to give a function all the arguments it has in it's type
    - No syntatic sugar, no preloaded libraries
        - ...though I have Prelude on my PC that i include for complex tests
    - Only simple include, no multiple inclusion checks, qualified includes and features like that. Only vanilla C-like straight up text inclusion
    - No comments for now and it is not simple enough to add to make me do it soon
    - Parsing errors don't contain position in code, but parsers have part of the information needed

Available features:
    - GHCI's 'it' keyword that contains result of the previous succsessful evaluation
    - Line editing and commands history via GNU readline

For most of the other things this interpretator's language is similar to Haskell
