# Digit-Analyser
Analyse digits using a genetic algorithm

Right before diving into machine learning I have decided to 
learn a little bit about genetic algorithms.

This software uses randomly generated patterns and assigns weights to them,
then filters the best patterns and uses them to generate new generation of patterns.

The accuracy I was able to reach was about 75% with the software mostly confusing number 9 with
number 3. 
Considering a relatively small dataset, I am content with the results.
Sadly the software Is quite slow, I will surely try a different approach next time,
also there is a problem with saving that I never had time to solve, so right now
learning has to be re-run every time.

This software was written with OpenGl and C++11 and using GLM, Perlin Noise and STBI libraries.

![example](https://raw.githubusercontent.com/Szustarol/Digit-Analyser/master/digitshow.png)
