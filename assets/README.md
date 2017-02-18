### About Free Culture Licenses

Loopidity source code, binary assets, and documentation are distributed as
[Free Software](https://www.gnu.org/philosophy/free-sw.html) and [Free Culture](http://freedomdefined.org/Definition).

"Free Culture" licenses such as
the [Crerative Commons licenses](https://creativecommons.org/share-your-work/licensing-types-examples/)
and the [Free Art License](http://artlibre.org/licence/lal/en/)
allow re-use and sharing of specific binary artifacts as long as attribution is preserved;
but they neglect ensuring of the freedom to study, experiment, and customize.
Experimentation implies de-composition; and as any artist or software developer knows:
non-trivial modifications require access to the original sources used by the author.
Without these sources, even the project maintainers are prevented from customizing the assets
beyond the most trivial operations such as trimming and scaling;
which is very much mis-aligned with the spirit of "Free Software".
Because they do not require the availability of the underlying sources that compose the artifacts,
these licenses are the analog of lax-permissive ("free as in beer") software licenses
and are not at all the natural companion to GPL-licensed software that they are often touted as.
The GPLv3 grants this maximal freedom of expression to a project's artists, developers, and end-users alike;
and is, itself, the natural companion license for the artistic binary assets of a GPL-licensed software program
provided that the relevant source materials are well-defined.


### Loopidity Assets License

The original audio, video, image and font files in the Loopidity assets/ directory
are licensed under [version 3 of the GNU General Public License](../COPYING) (GPLv3).
See [assets/LICENSES](LICENSES) for details.

The terminology in section 1 of the GPLv3 (namely: "source code", "preferred form", and "object code")
as it relates to the binary assets of this project is explicitly defined below.

The "object code" is explicitly defined here to be the binary audio, video, image and font files
accessed directly by the Loopidity program.

The "source code" or "preferred form of the work" is explicitly defined here to be any and all resources
(such as binary data, editor project files, meta-data, declarative texts, scripts, and source code of helper programs)
that are necessary to accomplish all of the following tasks using only widely-available free software:

* reconstruct the associated "object code" artifacts completely and accurately
* modify the fully decomposed "preferred form" sources directly and independently
* compose the original sources along with modified and replacement sources interchangeably
* generate equivalent modified versions of the associated artifacts

For example:

* "artifacts" such as a composed (mixed-down) .webm, .png, .ogg, .wav, etc
* "binary data" such are the individual elements that compose the "artifacts" (image layers, sound tracks, etc.)
* "meta-data" and "declarative texts" such as 3D models, animations, edit decision lists/cue sheets, CSS, etc.
* "scripts", and "source code of helpers" such as ImageMagick scripts, GIMP plugins, openGL shaders, etc.
* "editor project files" such as .blend, .xcf, .psd, .aup, .ardour, etc.
* "widely-available free software" such as Blender, GIMP, Inkscape, Audacity, Ardour, etc.


Exceptions:

* Purisa.ttf
  - Homepage: http://linux.thai.net/projects/fonts-tlwg
  - Sources:  https://github.com/tlwg/fonts-tlwg
  - License:  [version 2 or later of the GNU General Public License](COPYING.fonts)
