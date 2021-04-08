module GenTest;

import yitter.contract.IIdGenerator;

import std.conv;
import std.datetime;
import std.stdio;


class GenTest {

    private IIdGenerator IdGen;
    private int GenIdCount;
    private int WorkerId;

    this(IIdGenerator idGen, int genIdCount, int workerId) {
        GenIdCount = genIdCount;
        IdGen = idGen;
        WorkerId = workerId;
    }

    void GenStart() {
        MonoTime start = MonoTime.currTime();
        long id = 0;

        for (int i = 0; i < GenIdCount; i++) {
            id = IdGen.newLong();
        }

        MonoTime end = MonoTime.currTime();
        Duration dur = end - start;

        // writeln(id);
        writeln("++++++++++++++++++++++++++++++++++++++++WorkerId: "
                ~ WorkerId.to!string() ~ ", total: " ~ dur.total!("msecs").to!string() ~ " ms");

    }
}
