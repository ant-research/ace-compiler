---
layout: event
permalink: /events/home/
redirect_from: /events/index.html
title: ANT-ACE Tutorial at CGO 2025
description: 1st March 2025, Las Vegas, Nevada, USA
---

## [CGO 2025](https://2025.cgo.org/) Tutorial

### **Introduction**
**ANT-ACE** is currently being developed at the Ant Research Institute to establish a cutting-edge compiler ecosystem, specifically tailored to meet the diverse compilation needs of domain-specific languages and advanced computer architectures. ANT-ACE is dedicated to providing a stable and robust compiler infrastructure that features a flexible and extensible type system, complemented by robust support for domain-specific operations.

Leveraging the ANT-ACE compiler infrastructure, the Ant Research Institute has developed an FHE compiler designed to facilitate neural network inference on encrypted data. This compiler accepts ONNX models as input and produces optimized C/C++ programs as output, utilizing the institute's own open-source FHE library. Initial evaluations conducted on CPUs indicate that ANT-ACE delivers a 2.24x performance enhancement in processing ResNet models, surpassing the results of expert-coded implementations.

This tutorial is based on the CGO’25 Tool Paper titled "An FHE Compiler Framework for Automating Neural Network Inference," recently accepted for presentation at CGO’25. The primary goal of this tutorial is to introduce participants to the ANT-ACE compiler ecosystem, emphasizing the design objectives and foundational principles of the infrastructure. We will discuss how to enhance this infrastructure to create a domain-specific compiler. The session will provide a detailed examination of both the compiler infrastructure and the FHE compiler. Participants will learn how a pre-trained neural network model, provided in an ONNX file, is compiled into a Fully Homomorphic Encryption (FHE) program, enabling the performance of neural network inference on encrypted data. This thorough overview is designed to equip attendees with the knowledge necessary to understand and potentially extend the ANT-ACE ecosystem for their own domain-specific applications.

### **Tutorial Outline**

#### **1.  ANT-ACE Design Objectives, Principles, and Architecture**
   - **Compiler Infrastructure Design Objectives:** Overview of the goals driving the ANT-ACE development.
   - **Design Principles:** Core concepts and methodologies guiding the architecture and functionality.
   - **Architecture:** Detailed discussion on the multi-layered architecture, emphasizing continuous lowering through multiple levels of Intermediate Representation (IR) abstraction.

#### **2.  ANT-ACE for Automatic FHE Neural Network Inference**
   - **Compilation Pipeline Overview:** Step-by-step guide through the ANT-ACE FHE compiler’s workflow.
   - **FHE Compiler Front-End:** Introduction to the initial stages of the compilation process.
   - **Levels of IR:** Exploration of the five levels of IR used to compile a neural network computational graph into an FHE CKKS program.
   - **CKKS Parameter Selection:** Strategies for optimal parameter selection within the CKKS framework.
   - **FHE Noise Management:** Techniques for managing encryption noise to maintain computational accuracy.
   - **Debug Facilities:** Features for debugging including selective encryption, runtime tracing, and validation.
   - **More Research Areas:** FHE specific optimizations, new cryptographic algorithms and software-hardware codesign for accelerators

#### **4.  ANT-ACE FHE Compiler Extensions**


#### **3.  ANT-ACE Case Study and Demo: SecureChat via Llama Inference in FHE**
   - **PyTorch Integration:** How ANT-ACE interfaces with PyTorch to facilitate neural network modeling.
   - **Llama Operator Support:** How to extend ANT-ACE framework for specific support for new operators.
   - **Performance Issues:** Addressing performance bottlenecks and optimization strategies.
   - **Data Exploding Issue:** Tackling challenges related to data growth during computations.
   - **Demo:** How to download, build, test and debug ANT-ACE

### **Target Audience**
The target audience for this tutorial encompasses researchers and graduate students specializing in compiler technology and Fully Homomorphic Encryption (FHE), with particular interest in cryptographic application development, hardware accelerator research, and cryptographic algorithm research.


