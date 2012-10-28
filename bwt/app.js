var bwt = require("./bwt.js");
var fs = require("fs")

var infileName = process.argv[2];
var outfileName = process.argv[3];
var blockSize = parseInt(process.argv[4]);
 
console.dir("Using blocksize " + blockSize);
 
bwtOnFile(blockSize, infileName, outfileName);

function getFilesizeInBytes(filename) {
	 var stats = fs.statSync(filename)
	 var fileSizeInBytes = stats["size"]
	 return fileSizeInBytes
}
 
function bwtOnFile(blocksize, infile, outfile) {
	var finished = false;
	var alreadyProcessed = 0;
	var filesize = getFilesizeInBytes(infile);
	var lastPrinted = 0; //The last printed progress percentage
	//Open the file to write to
	fs.open(outfile, 'w', 0666, function(err, targetFD) {
		//Open a fd
		fs.open(infile, 'r', 0666, function(err, fd) {
				if(err) {console.dir(err);}
				//Read blocksize bytes
				while(1) {
					//Read the next blocksize bytes
					var buf = new Buffer(blocksize);
					var bytesRead = fs.readSync(fd, buf, 0, blocksize, null); //Read blocksize bytes
					//Calculate how much we processed so far
					var processedPercentage = 100*alreadyProcessed / filesize;
					if(processedPercentage >= lastPrinted+1) {
						lastPrinted += 1;
						console.dir("Processed " + lastPrinted + " percent of " + infile);
					}
					//Calculate the BWT
					var bwtBuf = bwt.bwtFromBuf(buf);
					var written = fs.writeSync(targetFD, bwtBuf, 0, bwtBuf.length, null);
					if(written != bwtBuf.length) {
						console.dir("Written only " + written + " of  " + bwtBuf.length + " bytes");
					}
					alreadyProcessed += bytesRead;
					//Break if the number of bytes read does not equal the expected bytecount (EOF)
					if(bytesRead != blocksize) {//Read blocksize bytes
						console.dir("Stopping...");
						break;
					}
				}
				fs.closeSync(fd);
				fs.closeSync(targetFD);
				console.dir("Finished");
				finished = true;
		});
	});
}
