# Code Samples

The code samples are composed from:

## CodeSamples 1

CodeSamples 1 contains solutions for the N-Body problem in 4 different ways:

1. Cuda straight forward implementation
  -  Spawning of a thread for every body in the system
2. Cuda implementation that decreases computations from O(N^2) to O(N^2/2)
  -  Through sharing of computations and a further step for gathering of results
3. C++ threads implementation making use of a producer and multiple consumers.
  - The producer contains a list of tasks
  - The consumers will continue to consume tasks until signalled
4. OpenMP implementation that makes use of the SSE3 instruction set to improve the performance

##CodeSample 2


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
