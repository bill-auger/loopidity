### Loopidity Contribution Guidelines

Loopidity source code, binary assets, and documentation are distributed as [Free Software](https://www.gnu.org/philosophy/free-sw.html) and [Free Culture](http://freedomdefined.org/Definition) under the terms of the [version 3 of the GNU General Public License](COPYING) (GPLv3). To this end, it is necessary for all contributors to be in agreement about licensing. This agreement is not a copyright assignment. It's function is merely to ensure that Loopidity remains free (as in freedom).

In short, this means that as a contributor, you allow others to copy, distribute, and modify their copies or your work; provided that they extend this priviledge to others and credit you as the original author. This implies that all source materials are made available in their respective preferred forms for modification; so for multimedia, this is all individual source files and/or editor project files in addition to the "mixed-down" binary artifacts. Refer to the 'Multimedia' section below for details.


### Contribution Licensing Terms

As a contributor, you agree that all contributions to the source tree, wiki, and issue tracker that are unlicensed and not in the public domain will automatically fall under the same licence as Loopidity.

If you are not the sole author of your contribution (i.e. it is a combined or derivative work incorporating or based on someone else's work), then all source works must be freely distributable under the terms of some [GPL-compatible license](https://www.gnu.org/licenses/license-list.html#GPLCompatibleLicenses) such as the Creative Commonns CC-BY or in the public domain; and you must clearly give attribution to the copyright holders with hyperlinks to the original sources and licenses.

This is the common "inbound<->outbound" tacet norm and is even made explicit by some public services such as [item 'D6' of the Github Terms of Service](https://help.github.com/articles/github-terms-of-service/#6-contributions-under-repository-license).

#### Multimedia and other Binary Contributions

For multimedia and other binaries, the licensing terms are explicitly defined in the [assets README](assets/README.md). Please read and understand this before submitting artwork and other prepared media. Contributions that do not meet the licensing criteria will not be accepted.


### Pull Requests

* Developers: please issue pull requests against the upstream 'development' branch.
* Designers: please issue pull requests against the upstream 'design' branch.

Note that branches other than 'master' tend to be rebased often so you may need to force pull those. Please rebase all pull requests onto the latest 'development' or 'design' HEAD and squash trivial commits but try to retain significant notable commits (see example below). Ideally in this way, all upstream branches should be a fast-foreward from 'master' and so re-sync should be simple.


### Repo Mirrors

Pull requests and issues are accepted at any of the following mirrors:

* https://github.com/bill-auger/loopidity/
* https://notabug.org/bill-auger/loopidity/
* https://pagure.io/loopidity/


### Commit Messages

Please add commit messages of the following form:
```
short general description of feature in declarative tense (optional issue #)
<EMPTY LINE>
  * specific notable change
  * specific notable change
  * specific notable change
  * specific notable change

e.g.

add bar powers to the mighty foo (issue #42)

  * added Bar class
  * added height, width instance vars to Bar class
  * added Bar instance var to Foo class
  * added setBar(), getBar() accessors to Foo class
```


### Issue Tracker

Please do not hesitate to use the issue tracker for bug reports, feature requests, constructive notes on existing issues, and significant progress updates that are not yet in a pull request; but use the gitter chat or other channels for lengthy discussions and other issues such as general help.
