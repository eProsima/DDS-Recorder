.. include:: ../../exports/alias.include

.. _getting_started_usage_example:

################
Example of usage
################

This example will serve as a hands-on tutorial, aimed at introducing some of the key concepts and features that
|eddsrecorder| has to offer.

Prerequisites
=============

It is required to have |eddsrecorder| previously installed using one of the following installation methods:

* :ref:`installation_manual_windows`
* :ref:`installation_manual_linux`
* :ref:`docker`

Additionally, `ShapesDemo <https://www.eprosima.com/index.php/products-all/eprosima-shapes-demo>`_ is required to publish and subscribe shapes of different colors and sizes.
Also, ShapesDemo is already prepared to use DynamicTypes, which is requiered when using the DDS Recorder.
Install it by following any of the methods described in the given links:

* `Windows installation from binaries <https://eprosima-shapes-demo.readthedocs.io/en/latest/installation/windows_binaries.html>`_
* `Linux installation from sources <https://eprosima-shapes-demo.readthedocs.io/en/latest/installation/linux_sources.html>`_
* `Docker Image <https://eprosima-shapes-demo.readthedocs.io/en/latest/installation/docker_image.html>`_

Start ShapesDemo
================

Let us launch a ShapesDemo instance and start publishing in topics ``Square`` with default settings.

.. figure:: /rst/figures/shapesdemo_publisher.png
    :align: center
    :scale: 75 %

Recorder configuration
======================

A |ddsrecorder| requires one YAML configuration file as the operation of this application is configured via this YAML configuration file.
|ddsrecorder| provide a configutation file named ``DDS_RECORDER_CONFIGURATION.yaml`` used by default if no configuration file is provided as argument, that must be in the same directory where the application is executed.
Therefore, the only argument required in the YAML config file right now is the ``filename`` which will be fixed to ``output`` by default.

.. note::
    Please refer to :ref:`Configuration <user_interface_configuration>` for more information on how to configure a recorder.

Recorder execution
==================

Launching a |ddsrecorder| instance is as easy as executing the following command:

.. code-block:: bash

    ddsrecorder

In order to know all the possible arguments supported by this tool, use the command:

.. code-block:: bash

    ddsrecorder --help or ddsrecorder -h

.. figure:: /rst/figures/shapesdemo_exec.png

Stop the recorder with ``Ctrl+C`` and check that the MCAP file exists.

Next Steps
==========

Explore section :ref:`Tutorials <tutorials_basic_example>` for more information on how to configure and set up a recorder, as well as to discover multiple scenarios where |ddsrecorder| may serve as a useful tool.