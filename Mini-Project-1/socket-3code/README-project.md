# Socket - Tri-Code Interoperability

Welcome to our first mini-project for 275. Here we explore the 
foundation of distributed computing - sockets. **Or how to 
communicate between processes.** 

Within the project you will find three sub-projects each using a different
programming language - Java, C++, and Python. They are of course named
`cpp-src, java-src,` and `python-src`. Each are in varying degrees of 
completeness. For instance, the cpp project is the closer in reaching 
stability, and the java the least. Each sub-project follows a similar 
organization to help with understanding. 

**A second objective is interoperability** across client and servers;
how all three languages support a common message construct.

An interoperability matrix, below, summarizes the lab's 
objectives for you to complete. The completed (Y) denotes
where rudimentary examples exist; they are far from 
being performant and hardened:


   | Clt/Svr  | Java     | Python3  | C/C++    |
   | -------- | :------: | :------: | :------: |
   | Java     | Y        |          |          | 
   | Python3  |          | Y        | Y        | 
   | C/C++    |          | Y        | Y        | 


Regarding features. It can be tempting to extend functionality
with features like rooms/groups, rankings, authentication, hashtags;
a miriad of shinny, interesting stuff. This is not the focus, and
does not affect your graded. In some cases, it may actuall take away 
from it, i.e if objectives are diminished or compromised by features.


### Sub-projects

It may help to view each sub-project as a standalone project. While 
grouped within the socket-3code directory, they are distinct. Imagine 
three geographically distinct teams coming together to pool efforts to
create the next greatest discord platform.

Last thing, each sub-project contains a `README.md`, you should read 
them.


### Language and Tools

The following is a list of languages and tools you will need to complete 
the project:

   Languages and Libraries
   * C/C++ (both)
     * gcc/g++, v13 or newer
     * Clang (not apple's xcode) v16 or newer
   * Java, v11 or newer
   * Python3

   Tools
   * cmake (https://cmake.org)
   * Linters for C/C++ and Python
   * lmod (https://github.com/TACC/Lmod)
   * Editors (2+): neovim/vim, vscode, IDEs, etc.
   * Terminal: kitty, iterm2, OS native, etc
   * You may want/need a font (https://www.nerdfonts.com) or two
   * Optional: Valgrind suite, perf, or other like tools

It is encouraged to use an application installer (`apt-get, brew`) for
provisioning your computer. 


### Tasks

Seed projects (`cpp-src, java-src,` and `python-src`) are provided to 
assist in coding. They each use a similar organization to allow for 
comparison of languages. Please feel free to modify, rename, delete what 
you need.

To expand on the introduction, we have three goals for the project:

  * First is to complete and harden (edge/corner cases) the 
    connectivity matrix (above) of the three projects. All combinations 
    are to be verified and hardened.

      * Write better code using linters, analysers, and sanitizers 
      * What are your edge and corner cases?
      * Hint: You may want to (Verify) starting with the inter-languages 
        (e.g., py-to-py) before the intra-language (e.g., py-to-cpp). 

  * Second, you are to measure performance and throughput. Document
    both successes and failures in your report and code. 

  * Lastly (optional), run the client(s) and server(s) on different 
    computers using a switch and cat6 (or 5) cables to connect the 
    two computers to the switch. **Do not use wifi.**

It's worth mentioning again, as tempting as it is and often a trait 
of engineers, adding features is not required. Spending effort to add 
creative or cool features is not the trust of the lab, and certainly
not the focus of grading.


### Restrictions and Limits

As this lab has been constructed with a specific set of learning and
discovery objectives, some aspects are artifically constrained. Yes,
many can be engineered out and exist in the wild yet, yet, the concepts 
remain valid. Therefore, there are several creativity boundaries that 
need to be put in place:

  * No third party libraries or services other than what has been 
    listed above.
  * The underlining direct use of socket cannot be replaced.
  * Circumventing the spirit of the lab.


### What to turn in

A `tar.gz` file containing a folder with 1) code, 2) report, and 
3) presentation. 

Source should include your project, test cases, test data, and
findings data.

The presention is **exactly one slide** (envision poster) to be
presented to the class.

The report is the support information behind the slide. This is 
your team's conclusions with data (tabular and graph formats),
observations, where you hardened the codes, performance solutions,
member contributions, and references/citations. 

Performance and hardening solutions irregardless whether they 
helped should be documented and explained. 

### What about A.I.?

Well, this has been the elephant in the room for many. So, how 
do we proceed. This will be interesting. If you choose to use 
A.I. to help, know this.

   * Your skills to use A.I. verses knowing how to research, and
     code is the tension that you must balance.
   * Demarked in code which is A.I., failure to do nulls the 
     assignment
   * Reports also must be demarked and citations used.
   

## Explore

   * Other framing (message) technologies?
   * Additional language tools 
   * Scaling, what code changes would you enact for many connections, 
     large volumes, or high traffic? * Chaining servers, diffusion, 
     aggregation, replication 
   * Proof that your code works across computers (no wifi) 
   * Building third party tools/libs from source using autoconf.
