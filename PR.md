# PR prefixes

The following tags should be added to their respective PR/ titles.

- **[Feature]**</br>
A new functionality to the engine was added, but this did not change the output of other functionalities. Internal changes to how methods give their output can be changed, but their outputs should stay the same.

- **[Improvement]**</br>
A change has been made on a method(s)/class(es) that either optimized the runtime performance, or the readability has been improved. This includes internal comments that are not made for documentation purposes but rather to explain the method/class.

- **[Build]**</br>
A change was made to the build structure. This includes, Static libraries turned into Shared ones, a warning/linker flag/compile definition was added etc.

- **[ThirdParty]**</br>
A new third party library was added. This PR should only include the library's addition to the third party folder and not it's integration. The integration part should be in another PR with the "[Feature]" tag.

- **[Bugfix]**</br>
Self explanatory, these PR's should include bugfixes.
