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

Code sample 2 contains the work of my Master's thesis, which involves the field of high-fidelity rendering, distributed systems, parallelism and distributed systems.



![alt text](GithubPagesAssits/system_overview.png)



The code can be split into two; the rendering component and the communication component, as specified in the

### A copy of my dissertation can be acquired from [here](./GithubPagesAssits/main.pdf)

## Compiling the Sources
To compile all the sources an environmental variable must be setup, specifying a directory containing the Boost Library

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
