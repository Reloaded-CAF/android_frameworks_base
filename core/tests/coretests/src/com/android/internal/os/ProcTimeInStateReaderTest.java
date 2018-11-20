/*
 * Copyright (C) 2018 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.internal.os;

import static org.junit.Assert.assertArrayEquals;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.fail;

import android.content.Context;
import android.os.FileUtils;
import android.support.test.InstrumentationRegistry;
import android.support.test.filters.SmallTest;
import android.support.test.runner.AndroidJUnit4;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;

import java.io.File;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;

@SmallTest
@RunWith(AndroidJUnit4.class)
public class ProcTimeInStateReaderTest {

    private File mProcDirectory;

    @Before
    public void setUp() {
        Context context = InstrumentationRegistry.getContext();
        mProcDirectory = context.getDir("proc", Context.MODE_PRIVATE);
    }

    @After
    public void tearDown() throws Exception {
        FileUtils.deleteContents(mProcDirectory);
    }

    @Test
    public void testSimple() throws IOException {
        Path initialTimeInStateFile = mProcDirectory.toPath().resolve("initial-time-in-state");
        Files.write(initialTimeInStateFile, "1 2\n3 4\n5 6\n7 8\n".getBytes());
        ProcTimeInStateReader reader = new ProcTimeInStateReader(initialTimeInStateFile);

        assertArrayEquals(
                "Reported frequencies are correct",
                new long[]{1, 3, 5, 7},
                reader.getFrequenciesKhz());
        assertArrayEquals(
                "Reported usage times are correct",
                new long[]{20, 40, 60, 80},
                reader.getUsageTimesMillis(initialTimeInStateFile));
    }

    @Test
    public void testDifferentFile() throws IOException {
        Path initialTimeInStateFile = mProcDirectory.toPath().resolve("initial-time-in-state");
        Files.write(initialTimeInStateFile, "1 2\n3 4\n5 6\n7 8\n".getBytes());
        ProcTimeInStateReader reader = new ProcTimeInStateReader(initialTimeInStateFile);

        Path timeInStateFile = mProcDirectory.toPath().resolve("time-in-state");
        Files.write(timeInStateFile, "1 20\n3 40\n5 60\n7 80\n".getBytes());
        assertArrayEquals(
                "Reported usage times are correct",
                new long[]{200, 400, 600, 800},
                reader.getUsageTimesMillis(timeInStateFile));
    }

    @Test
    public void testWrongLength() throws IOException {
        Path initialTimeInStateFile = mProcDirectory.toPath().resolve("initial-time-in-state");
        Files.write(initialTimeInStateFile, "1 2\n3 4\n5 6\n7 8\n".getBytes());
        ProcTimeInStateReader reader = new ProcTimeInStateReader(initialTimeInStateFile);

        Path timeInStateFile = mProcDirectory.toPath().resolve("time-in-state");
        Files.write(timeInStateFile, "1 2\n3 4\n5 6\n".getBytes());
        assertNull(reader.getUsageTimesMillis(timeInStateFile));
    }

    @Test
    public void testEmptyInitialFails() throws IOException {
        Path initialTimeInStateFile = mProcDirectory.toPath().resolve("initial-time-in-state");
        Files.write(initialTimeInStateFile, "".getBytes());
        try {
            new ProcTimeInStateReader(initialTimeInStateFile);
            fail("Instantiation didn't fail with empty initial time_in_state file");
        } catch (IOException ignored) {
        }
    }
}
