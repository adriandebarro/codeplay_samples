# Code Samples

The code samples are composed from:

## Sample 1

Code sample 1 contains 4 different solutions to solve the nbody problem:

1. Cuda straight forward implementation
  -  Spawning of a cuda thread for every body in the system
2. Cuda implementation that decreases computations from `O(N^2)` to `O(N^2/2)`
  -  Through sharing of computations and a further step for gathering of results
3. C++ threads implementation making use of a producer and multiple consumers.
  - The producer contains a list of tasks
  - The consumers will continue to consume tasks until signalled
  - Producer and Consumers operate in a lock-step fashion
4. OpenMP implementation
  - Makes use of the SSE3 instruction set for improved performance

## Sample 2
Code sample 2 contains the work of my Master's thesis, which involves the fields of high-fidelity rendering, distributed systems and high performance computing. The dissertation builds upon the research of Bugeja et al. in the following paper().

The original authors came out with the idea of collaborative HFR, where peers collaborating in the same scene; such as, peers in an online multiplayer game ----



### System Architecture

![alt text](GithubPagesAssets/peer_overview.PNG)

The System is composed from two major components(as shown in the figure above); the rendering component, which delivers physically correct illumination through the Irradiance Cache. The irradiance cache is a physically correct technique able to improve the time taken to compute diffuse inter-reflections, through the re-use of previously computed irradiance samples. On the other hand, the system makes use of a peer-to-peer architecture for the sharing of computed irradiance samples and data is disseminated through a network through the use of the Anti-Entropy technique, a technique which mimics how viruses spread in biological populations.

The state of the art in collaborative HFR happened to have questions related to the scalability of the technique. Moreover, peers do not discriminate tp whom data is passed (Since the Anti-Entropy) always picks a random peer from the sub-set of local peers.


![alt text](GithubPagesAssets/system_overview.PNG)

### Contribution 1

### Contribution 2

### Contribution 3

![alt text](GithubPagesAssets/town.png)


### Improving Sample 2



### A copy of my dissertation can be acquired from [here](./GithubPagesAssets/main.pdf)

## Compiling the Sources
To compile all the sources an environmental variable `BOOST_DIR` must be setup, specifying a directory containing a Boost Library

## Feedback
Any feedback on how to improve the code further would be greatly appreciated.

<!-- ## Welcome to GitHub Pages

You can use the [editor on GitHub](https://github.com/adriandebarro/codeplay_samples/edit/master/README.md) to maintain and preview the content for your website in Markdown files.

Whenever you commit to this repository, GitHub Pages will run [Jekyll](https://jekyllrb.com/) to rebuild the pages in your site, from the content in your Markdown files.

### Markdown

Markdown is a lightweight and easy-to-use syntax for styling your writing. It includes conventions for

```markdown
Syntax highlighted code block

# Header 1
## Header 2
### Header 3

- Bulleted
- List

1. Numbered
2. List

**Bold** and _Italic_ and `Code` text

[Link](url) and ![Image](src)
```

For more details see [GitHub Flavored Markdown](https://guides.github.com/features/mastering-markdown/).

### Jekyll Themes

Your Pages site will use the layout and styles from the Jekyll theme you have selected in your [repository settings](https://github.com/adriandebarro/codeplay_samples/settings). The name of this theme is saved in the Jekyll `_config.yml` configuration file.

### Support or Contact

Having trouble with Pages? Check out our [documentation](https://help.github.com/categories/github-pages-basics/) or [contact support](https://github.com/contact) and we’ll help you sort it out. -->
