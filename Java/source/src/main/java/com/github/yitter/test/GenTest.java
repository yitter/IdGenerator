package com.github.yitter.test;

import com.github.yitter.contract.IIdGenerator;

import java.util.HashSet;
import java.util.Set;

public class GenTest {

    private IIdGenerator IdGen;
    private int GenIdCount;
    private int WorkerId;
    private Set IdSet = new HashSet();

    public GenTest(IIdGenerator idGen, int genIdCount, int workerId) {
        GenIdCount = genIdCount;
        IdGen = idGen;
        WorkerId = workerId;
    }

    public void GenStart() {
        long start = System.currentTimeMillis();
        long id = 0;

        for (int i = 0; i < GenIdCount; i++) {
            id = IdGen.newLong();
            // IdSet.add(id);
        }

        long end = System.currentTimeMillis();
        long time = end - start;

        System.out.println(id);
        System.out.println("++++++++++++++++++++++++++++++++++++++++WorkerId: "
                + WorkerId + ", total: " + time + " ms");

    }
}
