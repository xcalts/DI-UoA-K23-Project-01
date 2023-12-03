<!-- Improved compatibility of back to top link: See: https://github.com/xcalts/project-one-DI352/pull/73 -->

<a name="readme-top"></a>

<!--
*** Thanks for checking out the Best-README-Template. If you have a suggestion
*** that would make this better, please fork the repo and create a pull request
*** or simply open an issue with the tag "enhancement".
*** Don't forget to give the project a star!
*** Thanks again! Now go create something AMAZING! :D
-->

<!-- PROJECT SHIELDS -->
<!--
*** I'm using markdown "reference style" links for readability.
*** Reference links are enclosed in brackets [ ] instead of parentheses ( ).
*** See the bottom of this document for the declaration of the reference variables
*** for contributors-url, forks-url, etc. This is an optional, concise syntax you may use.
*** https://www.markdownguide.org/basic-syntax/#reference-style-links
-->

[![Contributors][contributors-shield]][contributors-url]
[![Forks][forks-shield]][forks-url]
[![Stargazers][stars-shield]][stars-url]
[![Issues][issues-shield]][issues-url]

<!-- PROJECT LOGO -->
<br />
<div align="center">
  <h3 align="center">Search and Clustering in C/C++</h3>
  <p align="center">
    <a href="https://github.com/xcalts/project-one-DI352/issues">Report Bug</a>
    ·
    <a href="https://github.com/xcalts/project-one-DI352/issues">Request Feature</a>
  </p>
</div>

## Documentation of Project 2

|           | tAverageApproximate | tAverageBrute | MAF     |
|-----------|---------------------|---------------|---------|
| LSH       | 0.00582048          | 0.0278622     | 2.67399 |
| HyperCube | 0.00510190          | 0.0254808     | 4.19436 |
| GNNS      | 0.0179729           | 0.0275327     | 1.84797 |
| MNRG      | 0.00555143          | 0.0270995     | 2.94731 |

Τα παραπάνω αποτελέσματα προέκυψαν από εκτελέσεις με τα ορίσματα που περιγράφονται και πιο κάτω.

Όπως είναι αναμενόμενο, ο GNNS έχει το MAF, αλλά είναι και ο πιο αργός αλγόριθμος (όσο αφορά την εκτέλεση ενός query). Σε αντιπαραβολή με την ταχύτητα και την απόδοση του MRNG, αυτό οφείλεται στο γεγονός ότι από την φύση του ο MRNG αποτρέπει την δημιουργία ορισμένων μονοπατιών κατά την δημιουργία του γράφου, που στο GNNS θα υπήρχαν κανονικά. Έτσι κατά την φάση του query ο έλεγχος γίνεται πάνω σε περισσότερα πιθανά μονοπάτια στην περίπτωση του GNNS, με αποτέλεσμα να υπερισχύει μεν σε απόδοση από τον MRNG (μικρότερο MAF), αλλά να είναι αρκετά πιο αργός.

Ο LSH και ο Hypercube είναι αναμενόμενα πιο γρήγοροι από τον GNNS, αλλά προφανώς λιγότερο αποδοτικοί λόγω αυτού. Ενδεικτικά στην περίπτωση του LSH, η χρήση του στην υλοποίηση του GNNS και του MRNG έγινε με τα ίδια ακριβώς ορίσματα (hashing functions και hashing tables), οπότε η απόκλιση ανάμεσα στις 3 μεθόδους δεν οφείλεται στα επιμέρους ορίσματα των εκτελέσεων τους.  

Όπως ήταν αναμενόμενο, ο Hypercube είναι πιο γρήγορος από τον LSH, αλλά λιγότερο αποδοτικός.

## Requirements

```sh
$ sudo apt install build-essential
```

## Usage

```sh
$ make debug
$ ./bin/lsh -i data/input.1K.dat -q data/query.1K.dat -o output/results_lsh.txt --hash-function 15 --hash-tables 10 --num-nearest 2 -R 0
$ ./bin/cube -i data/input.1K.dat -q data/query.1K.dat -o output/results_cube.txt --num-nearest 2 -M 100 -probes 10 -k 14
$ ./bin/cluster -m lsh -i data/train-images.idx3-ubyte -q data/t10k-images.idx3-ubyte -o results.txt -c ./data/cluster.conf
$ ./bin/graph_search -i ./data/input.1K.dat -q ./data/query.1K.dat -o output/output_gnns.txt -m 1 -R 5 -N 2
$ ./bin/graph_search -i ./data/input.1K.dat -q ./data/query.1K.dat -o output/output_mrng.txt -m 2 -l 30 -N 2

```

## Contributing

Contributions are what make the open source community such an amazing place to learn, inspire, and create. Any contributions you make are **greatly appreciated**.

If you have a suggestion that would make this better, please fork the repo and create a pull request. You can also simply open an issue with the tag "enhancement".
Don't forget to give the project a star! Thanks again!

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## License

Distributed under the MIT License. See `LICENSE` for more information.

## Information

- Κυπραίος Χρήστος, 1115202000105
- Καλτσάς Χρήστος, 1115202000289

<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->

[contributors-shield]: https://img.shields.io/github/contributors/xcalts/project-one-DI352.svg?style=for-the-badge
[contributors-url]: https://github.com/xcalts/project-one-DI352/graphs/contributors
[forks-shield]: https://img.shields.io/github/forks/xcalts/project-one-DI352.svg?style=for-the-badge
[forks-url]: https://github.com/xcalts/project-one-DI352/network/members
[stars-shield]: https://img.shields.io/github/stars/xcalts/project-one-DI352.svg?style=for-the-badge
[stars-url]: https://github.com/xcalts/project-one-DI352/stargazers
[issues-shield]: https://img.shields.io/github/issues/xcalts/project-one-DI352.svg?style=for-the-badge
[issues-url]: https://github.com/xcalts/project-one-DI352/issues
[license-shield]: https://img.shields.io/github/license/xcalts/project-one-DI352.svg?style=for-the-badge
[license-url]: https://github.com/xcalts/project-one-DI352/blob/master/LICENSE
