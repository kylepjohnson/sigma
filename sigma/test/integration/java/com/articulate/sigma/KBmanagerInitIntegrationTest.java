package com.articulate.sigma;

import org.junit.Test;

import static org.junit.Assert.*;

public class KBmanagerInitIntegrationTest extends IntegrationTestBase {

    /**
     * Help verify that the correct config file is being run by checking how many kif files have been loaded. You could think
     * you're running with a new config file when you are not by, for example, modifying the test config file without
     * doing a full build (which puts the new config file into the build output directory). This test isn't great, but it
     * may save developers time when they encounter unexpected results.
     */
    @Test
    public void testNbrKifFilesLoaded()   {

        int expected = 37;
        int actual = SigmaTestBase.kb.constituents.size();
        assertEquals(expected, actual);
    }

    /**
     * Verify how long the base class's KBmanager initialization took.
     */
    @Test
    public void testInitializationTime()   {
        assertTrue("Actual time = " + IntegrationTestBase.totalKbMgrInitTime, IntegrationTestBase.totalKbMgrInitTime < 250000);
        // Just in case something whacky is going on, make sure it's greater than some minimum, too.
        assertTrue("Actual time = " + IntegrationTestBase.totalKbMgrInitTime, IntegrationTestBase.totalKbMgrInitTime > 50000);
    }
}