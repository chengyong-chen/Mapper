﻿//
// Author:
//   Michael Göricke
//
// Copyright (c) 2019
//
// This file is part of ShapeConverter.
//
// ShapeConverter is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program. If not, see<http://www.gnu.org/licenses/>.

using EpsSharp.Eps.Content;

namespace EpsSharp.Eps.Core
{
    /// <summary>
    /// The file operand
    /// </summary>
    internal class FileOperand : Operand
    {
        /// <summary>
        /// Constructor
        /// </summary>
        public FileOperand() : base("filetype")
        {
        }

        /// <summary>
        /// The file reader
        /// </summary>
        public EpsStreamReader Reader { get; set; }

        /// <summary>
        /// Create a copy
        /// </summary>
        protected override Operand CreateCopy()
        {
            var copy = new FileOperand();
            copy.Reader = Reader;

            return copy;
        }

        /// <summary>
        /// Returns a describing string for debugging purposes only
        /// </summary>
        public override string ToString()
        {
            return string.Format("file");
        }
    }
}
