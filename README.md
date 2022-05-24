<div id="top"></div>
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

<h3 align="center">Server Client Chatroom</h3>

  <p align="center">
    A centralized chatroom application.
    <br />
    <a href="https://github.com/AymaneElmahi/chatroom"><strong>Explore the docs »</strong></a>
    <br />
  </p>
</div>

<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about-the-project">About The Project</a>
      <ul>
        <li><a href="#built-with">Built With</a></li>
      </ul>
    </li>
    <li>
      <a href="#getting-started">Getting Started</a>
      <ul>
        <li><a href="#prerequisites">Prerequisites</a></li>
        <li><a href="#installation">Installation</a></li>
      </ul>
    </li>
    <li><a href="#usage">Usage</a></li>
    <li><a href="#contributions">Contributing</a></li>
    <li><a href="#contact">Contact</a></li>
  </ol>
</details>

<!-- ABOUT THE PROJECT -->

## About The Project

First of all we should note that this is a school project.
It's a "chatroom" application. It consists of a server and clients. It's like a message group on Whatsapp or messenger or whatever. Using the same locam IP adress and the same port, the clients can connect to the server and send messages to each other.

<p align="right">(<a href="#top">back to top</a>)</p>

### Built With

- C Language (C)

<p align="right">(<a href="#top">back to top</a>)</p>

<!-- GETTING STARTED -->

## Getting Started

### Prerequisites

To run this project, you need C language, using the command line.

### Installation

1. Clone the repo
   ```sh
   git clone https://github.com/AymaneElmahi/chatroom.git
   ```
2. make the project
   ```sh
   make
   ```

<p align="right">(<a href="#top">back to top</a>)</p>

<!-- USAGE EXAMPLES -->

## Usage

You can start the server by running the following command:

```sh
./server <port>
```

after of course choosing an empty port.

Then you can open a new terminal for example and start a new client by running the following command:

```sh
./client <ip> <port>
```

You can use the localhost ip address (127.0.0.1) and the port you chose.

by writing /help in the client terminal, you can see the available commands:

```sh
/help or /h
/quit or /q
/list or /l
/m <username> <message>
```

each command is explained after the /help command.

<p align="right">(<a href="#top">back to top</a>)</p>

<!-- CONTRIBUTING -->

## Contributions

This project was done by my collegue Mohamed Faid and myself Aymane Elmahi.

<p align="right">(<a href="#top">back to top</a>)</p>

<!-- CONTACT -->

## Contact

Mohamed Faid: faidm60@gmail.com

Aymane Elmahi: elmahi.aymane@gmail.com

Project Link: [https://github.com/AymaneElmahi/chatroom](https://github.com/AymaneElmahi/chatroom)

<p align="right">(<a href="#top">back to top</a>)</p>
