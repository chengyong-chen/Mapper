﻿//Released under the MIT License.
//
//Copyright (c) 2015 Ntreev Soft co., Ltd.
//
//Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated 
//documentation files (the "Software"), to deal in the Software without restriction, including without limitation the 
//rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit 
//persons to whom the Software is furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all copies or substantial portions of the 
//Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE 
//WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
//COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR 
//OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

using Ntreev.Library.Psd.Readers;
using System;
using System.IO;

namespace Ntreev.Library.Psd
{
    public class FileHeaderSection : LazyDataReader
    {
        public FileHeaderSection()
        {
        }

        public FileHeaderSection(PsdReader reader)
        {
          Init(reader, false, null);
        }

        public int Depth
        {
            get;
            set;
        }

        public int NumberOfChannels
        {
            get;
            set;
        }

        public ColorMode ColorMode
        {
            get;
            set;
        }

        public int Height
        {
            get;
            set;
        }

        public int Width
        {
            get;
            set;
        }

        protected override void ReadData(PsdReader reader, object userData)
        {
            NumberOfChannels = reader.ReadInt16();
            Height = reader.ReadInt32();
            Width = reader.ReadInt32();
            Depth = reader.ReadInt16();
            ColorMode = reader.ReadColorMode();
        }

        public static FileHeaderSection Read(PsdReader reader)
        {
            var instance = new FileHeaderSection(reader);
            return instance;
        }

        public static FileHeaderSection FromFile(string filename)
        {
            using (FileStream stream = new FileStream(filename, FileMode.Open, FileAccess.Read, FileShare.Read))
            using (PsdReader reader = new PsdReader(stream, new PathResolver(), new Uri(Path.GetDirectoryName(filename))))
            {
                reader.ReadDocumentHeader();
                return FileHeaderSection.Read(reader);
            }
        }
    }
}
