# QLux

QLux is a Geant4 simulation of a gas-argon time projection chamber instrumented with a 3 × 3 array of QL sensors. The simulation is used to study alpha-particle energy deposition, argon scintillation, photon transport, and the distribution of detected scintillation light across the QL sensor array.

The project includes detailed detector geometry, optical material properties, selectable QL sensor configurations, event-level photon recording, and optional energy-deposition output for subsequent charge-transport and detector-response analysis.

## Simulation

The default primary particle is a 5.304 MeV alpha, corresponding to the primary alpha emission from Po-210 and representing the Po-210 source used with the QLux detector. The alpha is emitted from a point source above the sensor plane, with directions generated isotropically over the downward-facing hemisphere.

The detector contains a gas-argon active volume above a 3 × 3 QL sensor array. Scintillation photons produced in the argon are transported through the detector geometry and recorded when they reach the active a-Se regions of the QL sensors.

The simulation currently supports two QL geometries:

* `Little`
* `Big`

The geometry can be selected before initialization using the `/QLux/Geometry` command.

## Output

QLux can generate several event-matched binary data products.

### Photon data

Photon output is recorded for each simulated event and contains:

* event ID
* total number of scintillation photons produced
* number of detected photons on each of the nine QL pixels

The photon output filename can be selected with:

`/QLux/PhotonFileName filename.bin`

The default filename is `photons.bin`.

### Primary alpha energy deposition

Energy deposition along the primary alpha track can optionally be recorded for use in charge-transport calculations.

Enable this output with:

`/QLux/EDepOutput true`

and select the output filename with:

`/QLux/EDepFileName filename.bin`

The default filename is `dEdx.bin`.

### Secondary-particle energy deposition

Energy deposition from secondary particles can also be recorded separately when required.

Enable this output with:

`/QLux/SecondaryOutput true`

and select the output filename with:

`/QLux/SecondaryFileName filename.bin`

The default filename is `secondary_edep.bin`.

Primary and secondary energy-deposition output are disabled by default.

## Building

QLux has been developed and tested using Geant4 11.0.3 with the required Geant4 data libraries installed and configured.

From the repository directory:

`mkdir build`

`cd build`

`cmake ..`

`make`

This produces the `QLux` executable in the build directory. Runtime macro and geometry files are copied into the build directory automatically by CMake.

## Running

Running the executable without arguments starts the Geant4 interactive visualization environment:

`./QLux`

A macro can be supplied for batch operation:

`./QLux macro.mac`

An example configuration is provided in `dedx.mac`.

A typical simulation macro selects the detector geometry, initializes Geant4, specifies the desired output files, configures the source position, and starts the requested number of events.

## Software

QLux is built using the Geant4 simulation toolkit and incorporates code and application patterns adapted from Geant4 examples.

CAD geometry is imported using CADMesh.

## License

See `LICENSE.txt` for licensing information. 
