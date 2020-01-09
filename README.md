# SQLiteImgDoc

This is an experiment - in order to evaluate the feasibility of using [SQLite](https://www.sqlite.org/index.html) as the persistance-layer (or file-format) for multi-dimensional images (as encountered e.g. in microscopy, cf. [OME-TIFF](https://docs.openmicroscopy.org/ome-model/6.0.0/ome-tiff/index.html) or [BioFormats](https://www.openmicroscopy.org/bio-formats/)).

## Design considerations

The images we are dealing with are organized as large mosaics with a pyramid.
![pyramid image](doc/pyramid.png)

For a viewer capable of fast zooming and panning, it is important to quickly retrieve the tiles intersecting with the specific viewport. For this task the [R*Tree module](https://sqlite.org/rtree.html) of SQLite seems well suited and is employed.

The bitmap data itself is being stored inside the SQLite-database (as a [BLOB](https://www.sqlite.org/datatype3.html)). This is a very controversial approach, and there is a lot of discussion what the best architecture for dealing with large blobs in databases is (e.g. [here](https://dba.stackexchange.com/questions/2445/should-binary-files-be-stored-in-the-database), [here](https://www.sqlite.org/intern-v-extern-blob.html)). So, getting some real-world experience here is one goal of this experiment.

The same techniques (ie. image pyramids and tiling) are applied to 3D-volumetric data.

This library focuses solely on the storage aspect; image processing aspects are out-of-scope and they are expected to be implemented on another layer.

## Q & A

**Why should image data be stored in a database? What is wrong with traditional file-formats like TIFF?**

Some of the advantages expected from using RDBMS (and SQLite in particular) are:
* consistent and unambigous data structure
* The database enables us to have the data indexed so that queries are fast. In particular, those indexes are managed by the database and they are persistent.
* Query operations can be formulated in SQL.
* Transparency and self-documentation - the data is easily accessible and the storage format is future proof (see [here](https://sqlite.org/locrsf.html)).
* Relying on [tested-and-true technology](https://sqlite.org/mostdeployed.html).

**Why is the code in C++? Wouldn't C#, Python, Javascript, Go, D,... be preferable?**

Short answer - no. Main reasons being that interfacing with SQLite is best done in C/C++; C++ is mature, time-tested and offers excellent performance and it is portable to literally any platform. It is expected that bindings for languages of your choice can be built on top of the C++-layer.

### Prerequisites

What things you need to install the software and how to install them

```
Give examples
```

### Installing

A step by step series of examples that tell you how to get a development env running

Say what the step will be

```
Give the example
```

And repeat

```
until finished
```

End with an example of getting some data out of the system or using it for a little demo

## Running the tests

Explain how to run the automated tests for this system

### Break down into end to end tests

Explain what these tests test and why

```
Give an example
```

### And coding style tests

Explain what these tests test and why

```
Give an example
```

## Deployment

Add additional notes about how to deploy this on a live system

## Built With

* [SQLite](https://www.sqlite.org/index.html) - version 3.30.1 is being used
* [SQLiteCpp](https://srombauts.github.io/SQLiteCpp/) - lightweight C++ SQLite3 wrapper

## Contributing

Please read [CONTRIBUTING.md](https://gist.github.com/PurpleBooth/b24679402957c63ec426) for details on our code of conduct, and the process for submitting pull requests to us.

## Versioning

We use [SemVer](http://semver.org/) for versioning. For the versions available, see the [tags on this repository](https://github.com/your/project/tags). 

## Authors

* **Jürgen Bohl** - *Initial work* - [jbohl@h-quer.de](mailto:jbohl@h-quer.de)

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details
